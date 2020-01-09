// Copyright (c) 2014-2019, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#include "checkpoints.h"
#include <fstream>
#include "common/dns_utils.h"
#include "common/util.h"
#include "string_tools.h"
#include "storages/portable_storage_template_helper.h" // epee json include
#include "serialization/keyvalue_serialization.h"
#include <vector>
#include "libznipfs/json.hpp"
#include "libznipfs/HTTPRequest.hpp"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <cstring>
#include <boost/algorithm/string.hpp>
#ifdef __linux__
#include <unistd.h>
#include "libznipfs/libznipfs-linux.h"
#endif
#ifdef WIN32
#include <windows.h>
#include "libznipfs/libznipfs-windows.h"
#endif
#ifdef __APPLE__
#include <unistd.h>
#include "libznipfs/libznipfs-macos.h"
#endif

using namespace epee;
using nlohmann::json;

#undef SCALA_DEFAULT_LOG_CATEGORY
#define SCALA_DEFAULT_LOG_CATEGORY "checkpoints"

namespace cryptonote
{
  /**
   * @brief struct for loading a checkpoint from json
   */
  struct t_hashline
  {
    uint64_t height; //!< the height of the checkpoint
    std::string hash; //!< the hash for the checkpoint
        BEGIN_KV_SERIALIZE_MAP()
          KV_SERIALIZE(height)
          KV_SERIALIZE(hash)
        END_KV_SERIALIZE_MAP()
  };

  /**
   * @brief struct for loading many checkpoints from json
   */
  struct t_hash_json {
    std::vector<t_hashline> hashlines; //!< the checkpoint lines from the file
        BEGIN_KV_SERIALIZE_MAP()
          KV_SERIALIZE(hashlines)
        END_KV_SERIALIZE_MAP()
  };

  //---------------------------------------------------------------------------
  checkpoints::checkpoints()
  {
  }
  //---------------------------------------------------------------------------


  bool checkpoints::add_checkpoint(uint64_t height, const std::string& hash_str)
  {
    crypto::hash h = crypto::null_hash;
    bool r = epee::string_tools::hex_to_pod(hash_str, h);
    CHECK_AND_ASSERT_MES(r, false, "Failed to parse checkpoint hash string into binary representation!");

    // return false if adding at a height we already have AND the hash is different
    if (m_points.count(height))
    {
      CHECK_AND_ASSERT_MES(h == m_points[height], false, "Checkpoint at given height already exists, and hash for new checkpoint was different!");
    }
    m_points[height] = h;
    return true;
  }
  //---------------------------------------------------------------------------
  bool checkpoints::is_in_checkpoint_zone(uint64_t height) const
  {
    return !m_points.empty() && (height <= (--m_points.end())->first);
  }
  //---------------------------------------------------------------------------
  bool checkpoints::check_block(uint64_t height, const crypto::hash& h, bool& is_a_checkpoint) const
  {
    auto it = m_points.find(height);
    is_a_checkpoint = it != m_points.end();
    if(!is_a_checkpoint)
      return true;

    if(it->second == h)
    {
      MINFO("CHECKPOINT PASSED FOR HEIGHT " << height << " " << h);
      return true;
    }else
    {
      MWARNING("CHECKPOINT FAILED FOR HEIGHT " << height << ". EXPECTED HASH: " << it->second << ", FETCHED HASH: " << h);
      return false;
    }
  }
  //---------------------------------------------------------------------------
  bool checkpoints::check_block(uint64_t height, const crypto::hash& h) const
  {
    bool ignored;
    return check_block(height, h, ignored);
  }
  //---------------------------------------------------------------------------
  //FIXME: is this the desired behavior?
  bool checkpoints::is_alternative_block_allowed(uint64_t blockchain_height, uint64_t block_height) const
  {
    if (0 == block_height)
      return false;

    auto it = m_points.upper_bound(blockchain_height);
    // Is blockchain_height before the first checkpoint?
    if (it == m_points.begin())
      return true;

    --it;
    uint64_t checkpoint_height = it->first;
    return checkpoint_height < block_height;
  }
  //---------------------------------------------------------------------------
  uint64_t checkpoints::get_max_height() const
  {
    std::map< uint64_t, crypto::hash >::const_iterator highest = 
        std::max_element( m_points.begin(), m_points.end(),
                         ( boost::bind(&std::map< uint64_t, crypto::hash >::value_type::first, _1) < 
                           boost::bind(&std::map< uint64_t, crypto::hash >::value_type::first, _2 ) ) );
    return highest->first;
  }
  //---------------------------------------------------------------------------
  const std::map<uint64_t, crypto::hash>& checkpoints::get_points() const
  {
    return m_points;
  }

  bool checkpoints::check_for_conflicts(const checkpoints& other) const
  {
    for (auto& pt : other.get_points())
    {
      if (m_points.count(pt.first))
      {
        CHECK_AND_ASSERT_MES(pt.second == m_points.at(pt.first), false, "Checkpoint at given height already exists, and hash for new checkpoint was different!");
      }
    }
    return true;
  }

  bool checkpoints::add_swarm_seed(){
  try
    {
    http_hayzu::Request requestSwarm("http://localhost:5001/api/v0/swarm/connect?arg=/ip4/149.91.88.54/tcp/4001/ipfs/QmXpo52G757vmEuyroVdXqPKNtJdwa7DMYwkehn2yFYB8b");
    const http_hayzu::Response getResponse = requestSwarm.send("GET");
    
    LOG_PRINT_L0("Added QmXpo52G757vmEuyroVdXqPKNtJdwa7DMYwkehn2yFYB8b as Swarm Seed!");
    return true;
    }
    catch (const std::exception& e)
    {
    
    LOG_PRINT_L0("Was not able to add QmXpo52G757vmEuyroVdXqPKNtJdwa7DMYwkehn2yFYB8b as Swarm Seed, will add some hardcoded checkpoints.");
    ADD_CHECKPOINT(1,"8fdac8eb91e8b35f3ed608a33fb446fca5d207095c97ce75090700175d22082f");
    ADD_CHECKPOINT(10,"246f96b4b6793f195715a95d911b95a957bc81a992450b19909ee6d5045c6911");
    ADD_CHECKPOINT(100,"443405c37b78bdfb3689c5c3081508a9c6872829ad0d6386f4e69e66add0252d");
    ADD_CHECKPOINT(1000,"d84377d3d61e2044c7f81708cc0914795e511f05c1b16236032209d901588fc1");
    ADD_CHECKPOINT(5000,"a983076ab629a5049d400149cdc3fc28f5618fd123289526fe433a8c5b102623");
    ADD_CHECKPOINT(10000,"236b093c6bcb437715195d6e145c734328bb18cad5ea44bc974bd1e7381eb21b");
    ADD_CHECKPOINT(25000,"9b4055bb2218565e29f8bb69a7e77ee477c426068d9711250d312365c612ba9f");
    ADD_CHECKPOINT(40000,"1316d1f24ad197f1cec2cfc5efd8b38c599a888261a9d3c0c7234eae30146938");
    ADD_CHECKPOINT(55000,"0a2bc0b33baad5ebcea35472774de07f9378f7e62c7abe8d22d630189853242c");
    ADD_CHECKPOINT(65000,"b345dc8e18ea05d4e3e98880759351c5e963c8165b53f6ee2efec69591f9170b");
    ADD_CHECKPOINT(67500,"01b603914e7907624db930e26e17eae1b86b6c903c86cb385f49257861a07192");
    ADD_CHECKPOINT(69331,"fd6d9e345079f682ec383e5c28272e88cc2c186e2fd49682e20ca74a1248b558");
    ADD_CHECKPOINT(75000,"26b95abb4a3292ab34bcf1b98b5ae415e415a7f3f51b1dad874504ab0d39b733");
    ADD_CHECKPOINT(85000,"5e336dbd3431e0cb0259b8077c4b5f5140b121f738f5fd3bcba09c07837e317c");
    ADD_CHECKPOINT(95000,"f6bc2524c016e0cbf2069688eec14ef80e8fd7eb75a449a63e13f83ed7bb5b7c");
    ADD_CHECKPOINT(100000,"167dccc73c4155d64f9cd764bf089bad51cbab3ab7a2a098984c40b66e3196a2");
    ADD_CHECKPOINT(100800,"e2f226fd416979a61bd4affb5406a5b6438786b7969c2c8ad6fe54f86bfb73b8");
    ADD_CHECKPOINT(100801,"8f1b2f081afd5f6d5e43425f0520a49e94ac9e0d78470e88d7b591a25772203a");
    ADD_CHECKPOINT(100805,"a7bce691d5426ada660d8835b05ca783ee4dd4f986e0ea157961f1b790c3beb9");
    ADD_CHECKPOINT(110000,"b6102ac6672ccfd0e13110e53ddf45a9a9396b565d74451452fa60ca784c1fc7");
    ADD_CHECKPOINT(120000,"3fcd577d3e9410b62278208a3895689ede331e4858e3570177b4aa77120adcd7");
    ADD_CHECKPOINT(130000,"a9674c50c492d1d69408161c455aa0983836153506b717d8e90502a959c650aa");
    ADD_CHECKPOINT(140000,"46f8f53789f73fefde87022afe1d4d1e805a09709b3c06d305c09019f5782823");
    ADD_CHECKPOINT(150000,"ad8282bc06037e297259ec1880af189a2507ad25ba7773825acdb38a25558b4c");
    ADD_CHECKPOINT(160000,"6ab0cec31984d3f9e3ec7187b93dd854fef31f9e0c93757e5ff17ab2712306e9");
    ADD_CHECKPOINT(170000,"e2f8a7005a0c43cda21cea93d27c89bf78321462a48f687d88c540e16fcf59b6"); 
    ADD_CHECKPOINT(180000,"33a6486c1877b538bdafd95c1e710f3d2f606b080bceb90a0dd9115bd51a4c17"); 
    ADD_CHECKPOINT(190000,"93eeb8f2b29e6233ce7f4374727c11f06e681c04434001e394354c91f596c6a9");
    ADD_CHECKPOINT(194600,"dd06ed56d3e9988bb9f1adbbcfc2fb8e9c701190f100a6cd55109ee7f75f1018");
    ADD_CHECKPOINT(194601,"5f0cc92657a9f80d02b1392c9f3b9344fb52f7221f0126a133ece264fc9c5876");
    ADD_CHECKPOINT(194602,"41c6cda526facb03adf0404c4e9239eeab44554ad54f9f41cf5d3d0d0b72f9b3");
    ADD_CHECKPOINT(194603,"371e81a3f5026da2015cd350dc220b28af55c311bc08e4bf5cf2594e181c6751");
    ADD_CHECKPOINT(194604,"1e8ae3c5ca29aed9eaeb4726b65fa218f049a9e7a9fb8e273746055ef5c074e0");
    ADD_CHECKPOINT(194605,"765078391a8d29d534d8fd8b427eda33d0a28cc03f405cd3ba3ce09b98d62e11");
    ADD_CHECKPOINT(194610,"4d2fb71ed78cd9fc0f810d0ae4c9255e90e04938103afd5e26eb1fa64562054c");
    ADD_CHECKPOINT(194700,"a71cd06fe590febde7dd2e0675e477e7a2c6938a29da57832b94ea18c293272f");
    ADD_CHECKPOINT(194800,"fae7c8d3e3bab92d6f64d37d206cb868eee2e4d03dd8afcb9a35551617a81201");
    ADD_CHECKPOINT(194900,"0a9cab6bbe98d8c4392edc2eec8e296daedfdc0b48d2e2f839d04f6952a5e299");
    ADD_CHECKPOINT(195000,"924114382dd4d3a0db8e76db92f737e532e8d756c5a7bb3e7a36a969f00eaab9");
    ADD_CHECKPOINT(196000,"31db26de70dc40c24ffca2cc5945ab400d8ebdf33d11f8762ae62f4827a9e3ce");
    ADD_CHECKPOINT(200000,"e0f511a7d0e61e155b66de5e0b4c1c576cfcc27e173b4e85d528e9b5f4e7fdbb");
    ADD_CHECKPOINT(250000,"0eb44a540f4f82ad1a981d1b8cf56a21073827409e95a07d91a44236455e677e");
    ADD_CHECKPOINT(300000,"34993ddc590530e7b541685de613af5bff8679c88805851d505289f9b988f5fa");
    ADD_CHECKPOINT(350000,"dde51c3b038af14c149bc69cbece9bdf4c2fe63043aef83a1f82d8e38c152993");
    ADD_CHECKPOINT(400000,"77bf10753f4da1b362f5d9b63aeac6e8d6d6290d32aece011a5567a3170c0cd7");
    ADD_CHECKPOINT(450000,"4b2f226a3a3b5def227dcfa630727914e6d711c21c799afa076f24b663c7deff");
    ADD_CHECKPOINT(500000,"bcd21818b70182dbf6916d960f81afb11fee31243fdc9cac8f2e9d47af73364e");
    ADD_CHECKPOINT(510000,"26b7baeb36741bc43ac862f265fd522f3cc6981d223271150ab1c6485ca7814b");
    ADD_CHECKPOINT(520000,"67090f7174808da16d5e06c8875d52f7292fb13e6013470583a4c36039bdc2d1");
    ADD_CHECKPOINT(530000,"f2b629fb6e423a0edce1631b7004d8b0267a1793f3edc6efee0f4e225f65b10f");
    ADD_CHECKPOINT(540000,"b0bce9c5870c3fc5512e02300b7782aea04ee82c784c092588708fb7521bc763");
    ADD_CHECKPOINT(550000,"e5e1e6a81038fe5169cf3ae0d4d6648fdf810208c09bd474ae18821ffde2d16e");
    ADD_CHECKPOINT(560000,"5045434f5547975660759e6047d6a51801278651e703d0e764e40acb44188de3");
    ADD_CHECKPOINT(561000,"ec0d155eeac0ba96f793d38c953d456b4bcd9356bfc94a9a9401515ebfa35982");
    ADD_CHECKPOINT(562000,"6340cdd5cee217fbbe2c6026784b9312abe63135b40ac7472803c7a002c69dc3");
    ADD_CHECKPOINT(563000,"57b74132e872d90a08903db2fe817e87a09ec1f807ef98f1de912894a652eb81");
    ADD_CHECKPOINT(563810,"17249e28de0a86bb6a8f075f1b8f1e433a59a1d6ef9748acaaf4d7657560705c");
    ADD_CHECKPOINT(563900,"e2dd4da9217873b54b120f24ea0817c0d62ccb99a6d0b6a47851440e52076ec8");
    ADD_CHECKPOINT(565743,"c24888266d9cc093a13c1d74e31176d1a710e695586c90183d975b674dadd257");
    ADD_CHECKPOINT(566200,"0fa4b56537d191932854fb2faf5fdd53931c0eb384e65abd7d0ec96accb721fe");
    ADD_CHECKPOINT(567222,"57a3acfc3c9ba353731cb3a3266e0de89d8ed0f700ae6d407d1481738a8d315a");
    ADD_CHECKPOINT(567507,"e385c6d652678a8f65dec43e95e0690fb6dd916c8913f048854e78c8d41d547c");

    return false;
    }
  }

  bool checkpoints::init_default_checkpoints(network_type nettype)
  {

    boost::filesystem::path scala_dir = tools::get_default_data_dir(); // Scala's Data Directory
    #ifdef __linux__
    std::string scala_dir_usable = boost::filesystem::canonical(scala_dir).string() + "/checkpoints.json";
    #endif
    #ifdef __darwin__
    std::string scala_dir_usable = boost::filesystem::canonical(scala_dir).string() + "/checkpoints.json";
    #endif
    #ifdef WIN32
    std::string scala_dir_usable = boost::filesystem::canonical(scala_dir).string() + "\\checkpoints.json";
    scala_dir_usable = boost::algorithm::replace_all_copy(scala_dir_usable,"/","\\");
    #endif

    try
    {
        http_hayzu::Request request("http://localhost:5001/api/v0/shutdown");
        const http_hayzu::Response getResponse = request.send("GET");
        LOG_PRINT_L0("A running IPFS instance was shutdown");
    }
    catch (const std::exception &e)
    {
        LOG_PRINT_L0("No IPFS instance detected, starting one now.");
        /*Start a new instance of IPFS */
        std::string location;
        char *c_location;

        #ifdef __linux__
          location = boost::filesystem::canonical(scala_dir).string();
          c_location = &(location[0]);
          LOG_PRINT_L0(IPFSStartNode(c_location));
        #endif
        #ifdef WIN32
          location = boost::filesystem::canonical(scala_dir).string();
          location = boost::algorithm::replace_all_copy(location,"/","\\");
          c_location = &(location[0]);
          LOG_PRINT_L0(IPFSStartNode(c_location));
        #endif
        #ifdef __darwin__
          location = boost::filesystem::canonical(scala_dir).string();
          c_location = &(location[0]);
          LOG_PRINT_L0(IPFSStartNode(c_location));
        #endif

        std::this_thread::sleep_for (std::chrono::seconds(10));
        bool swarm_result = add_swarm_seed();
        if(swarm_result == true){
        try{
                        /* Add the big init checkpoint file */
                        http_hayzu::Request request4("http://localhost:8080/ipfs/QmYWbeSx649S9nnm6WSTkYu6eoDwp9a3AmqRyTS1kBFkJ5/checkpoints.json");
                        const http_hayzu::Response getResponse4 = request4.send("GET");
                        std::string jsonResBig = std::string(getResponse4.body.begin(), getResponse4.body.end());
                        std::ofstream o(scala_dir_usable);
                        o << jsonResBig;
                        LOG_PRINT_L0("Added large checkpoint file from IPFS at hash QmYWbeSx649S9nnm6WSTkYu6eoDwp9a3AmqRyTS1kBFkJ5");
            }
            catch (const std::exception &e){
                        std::cerr << "IPFS GET Request failed, error: " << e.what() << '\n';
            }
      }
    else{
    LOG_PRINT_L0("Wasn't able to connect to a Swarm Node hence not fetching checkpoints, IPFS daemon will still work!");
    }
    }
    return true;
  }

  bool checkpoints::load_checkpoints_from_json(const std::string &json_hashfile_fullpath)
  {
    boost::system::error_code errcode;
    if (! (boost::filesystem::exists(json_hashfile_fullpath, errcode)))
    {
      LOG_PRINT_L1("Blockchain checkpoints file not found");
      return true;
    }

    LOG_PRINT_L1("Adding checkpoints from blockchain hashfile");

    uint64_t prev_max_height = get_max_height();
    LOG_PRINT_L1("Hard-coded max checkpoint height is " << prev_max_height);
    t_hash_json hashes;
    if (!epee::serialization::load_t_from_json_file(hashes, json_hashfile_fullpath))
    {
      MERROR("Error loading checkpoints from " << json_hashfile_fullpath);
      return false;
    }
    for (std::vector<t_hashline>::const_iterator it = hashes.hashlines.begin(); it != hashes.hashlines.end(); )
    {
      uint64_t height;
      height = it->height;
      if (height <= prev_max_height) {
	LOG_PRINT_L1("ignoring checkpoint height " << height);
      } else {
	std::string blockhash = it->hash;
	LOG_PRINT_L1("Adding checkpoint height " << height << ", hash=" << blockhash);
	ADD_CHECKPOINT(height, blockhash);
      }
      ++it;
    }

    return true;
  }

  bool checkpoints::load_dynamic_checkpoints()
  {

    boost::filesystem::path scala_dir = tools::get_default_data_dir(); // Scala's Data Directory
    #ifdef __linux__
    std::string scala_dir_usable = boost::filesystem::canonical(scala_dir).string() + "/checkpoints.json";
    #endif
    #ifdef __darwin__
    std::string scala_dir_usable = boost::filesystem::canonical(scala_dir).string() + "/checkpoints.json";
    #endif
    #ifdef WIN32
    std::string scala_dir_usable = boost::filesystem::canonical(scala_dir).string() + "\\checkpoints.json";
    scala_dir_usable = boost::algorithm::replace_all_copy(scala_dir_usable,"/","\\");
    #endif

    try
    {
    http_hayzu::Request requestIPNS("http://ipns.scalaproject.io/");
    const http_hayzu::Response getIPNS = requestIPNS.send("GET");
    std::string IPNS_json = std::string(getIPNS.body.begin(), getIPNS.body.end());
    json IPNS_json_parsed = json::parse(IPNS_json);
    std::string IPFS_location = IPNS_json_parsed["ipfs_hash"];
    std::string IPFS_location_url = "http://localhost:8080/ipfs/" + IPFS_location;

    http_hayzu::Request dynamicCheckpoints(IPFS_location_url);
    const http_hayzu::Response getDynamicCheckpoints = dynamicCheckpoints.send("GET");
    std::string jsonDC = std::string(getDynamicCheckpoints.body.begin(), getDynamicCheckpoints.body.end());
    std::ofstream o2(scala_dir_usable);
    o2 << jsonDC;
    LOG_PRINT_L0("Pulled IPNS based checkpoint file from IPFS!");
    return true;
    }catch(const std::exception& e){
      LOG_PRINT_L0("Could not pull IPNS based checkpoint, will try again later");
      std::cerr << "IPNS Request failed, error: " << e.what() << '\n';
      return true;
    }
  }

  bool checkpoints::load_checkpoints_from_dns(network_type nettype)
  {
    std::vector<std::string> records;

    // All four ScalaPulse domains have DNSSEC on and valid
    static const std::vector<std::string> dns_urls = { "checkpoints.scalaproject.io"
    };

    static const std::vector<std::string> testnet_dns_urls = { "testpoints.scalaproject.io"
    };

    static const std::vector<std::string> stagenet_dns_urls = { "stagenetpoints.scalaproject.io"
    };

    if (!tools::dns_utils::load_txt_records_from_dns(records, nettype == TESTNET ? testnet_dns_urls : nettype == STAGENET ? stagenet_dns_urls : dns_urls))
      return true; // why true ?

    for (const auto& record : records)
    {
      auto pos = record.find(":");
      if (pos != std::string::npos)
      {
        uint64_t height;
        crypto::hash hash;

        // parse the first part as uint64_t,
        // if this fails move on to the next record
        std::stringstream ss(record.substr(0, pos));
        if (!(ss >> height))
        {
    continue;
        }

        // parse the second part as crypto::hash,
        // if this fails move on to the next record
        std::string hashStr = record.substr(pos + 1);
        if (!epee::string_tools::hex_to_pod(hashStr, hash))
        {
    continue;
        }
        ADD_CHECKPOINT(height, hashStr);
      }
    }
    return true;
  }

  bool checkpoints::load_new_checkpoints(const std::string &json_hashfile_fullpath, network_type nettype, bool dns)
  {
    bool result;
    
    result = load_checkpoints_from_json(json_hashfile_fullpath);
    if (dns)
    {
      result &= load_checkpoints_from_dns(nettype);
    }
    return result;
  }
}

