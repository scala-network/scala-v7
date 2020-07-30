//Copyright (c) 2014-2019, The Monero Project
//Copyright (c) 2018-2020, The Scala Network
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

#include "common/dns_utils.h"
#include "string_tools.h"
#include "storages/portable_storage_template_helper.h" // epee json include
#include "serialization/keyvalue_serialization.h"
#include <vector>
#include <boost/random.hpp>
#include <iostream>
#include <ctime>
#include <cstdint>
#include <sstream>
#include <string>
#include <algorithm>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost::property_tree;
using namespace boost::algorithm;
using namespace epee;

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
   bool checkpoints::get_request_ldpow(const std::string& request_url, std::string& response, const std::string& log_message)
   {
     epee::net_utils::http::http_simple_client client;
     epee::net_utils::http::url_content u_c;

     if (!epee::net_utils::parse_url(request_url, u_c)){
        MERROR("Failed to parse URL " << request_url);
        return false;
     }

     if (u_c.host.empty()){
				MERROR("Failed to determine address from URL " << request_url);
        return false;
     }

     epee::net_utils::ssl_support_t ssl_requirement = u_c.schema == "https" ? epee::net_utils::ssl_support_t::e_ssl_support_enabled : epee::net_utils::ssl_support_t::e_ssl_support_disabled;
     uint16_t port = u_c.port ? u_c.port : ssl_requirement == epee::net_utils::ssl_support_t::e_ssl_support_enabled ? 443 : 80;

     client.set_server(u_c.host, std::to_string(port), boost::none, ssl_requirement);
     epee::net_utils::http::fields_list fields;
     const epee::net_utils::http::http_response_info *info = NULL;

		 if (!client.invoke_get(u_c.uri, std::chrono::seconds(5), "", &info, fields)){
				LOG_PRINT_L0(request_url << " is not responding, skipping.");
				return false;
		 }else{
     LOG_PRINT_L1(log_message);
     response = std::string(info->m_body);
     return true;
     }
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
    if (m_points.empty())
      return 0;
    return m_points.rbegin()->first;
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
  //---------------------------------------------------------------------------
  bool checkpoints::init_default_checkpoints(network_type nettype)
  {
    if (nettype == TESTNET)
    {
      return true;
    }
    if (nettype == STAGENET)
    {
      return true;
    }
    std::string request_url_serverless = "http://checkpoint.scalaproject.io/";
    std::string response_serverless = "";
    std::string log_message_serverless = "Sending request to GET IPFS hash for big checkpoint.";

    bool big_checkpoint_request_SL = get_request_ldpow(request_url_serverless, response_serverless, log_message_serverless);
    if(big_checkpoint_request_SL == true){
      std::string request_url_ipfs_gateway = "https://ipfs.io/ipfs/" + response_serverless;
      std::string response_ipfs_gateway = "";
      std::string log_message_ipfs_gateway = "Sending request to GET big checkpoint from IPFS Gateway.";
      bool ipfs_checkpoint = get_request_ldpow(request_url_ipfs_gateway, response_ipfs_gateway, log_message_ipfs_gateway);
      if(ipfs_checkpoint == true){
        std::string hash_string = "hash";
        if (response_ipfs_gateway.find(hash_string) != std::string::npos){

        std::stringstream string_stream(response_ipfs_gateway);
        ptree stree;
        read_json(string_stream, stree);
        uint64_t height;
        std::string hash;

        BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, stree) {
          height = v.second.get<int>("height");
          hash = v.second.get<std::string>("hash");

          MGINFO("Height " << height << " hash " << hash << " from IPFS.");
          ADD_CHECKPOINT(height, hash);
        }
        return true;
        }else{
          	MERROR("Skipping initializing IPFS checkpoints, something went wrong.");
				    return true;
        }
      }
    }else{
          	MERROR("Skipping initializing IPFS checkpoints, something went wrong.");
				    return true;
    }
    return true;
  }
  //---------------------------------------------------------------------------

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
  //---------------------------------------------------------------------------
  bool checkpoints::load_checkpoints_from_ldpow_network(uint64_t height){
    std::time_t now = std::time(0);
    boost::random::mt19937 gen{static_cast<std::uint32_t>(now)};
    boost::random::uniform_int_distribution<> dist{0, 15};

    std::vector<std::string> two_nodes;
    two_nodes.push_back(cryptonote::ldpow_nodes.at(dist(gen)));
    two_nodes.push_back(cryptonote::ldpow_nodes.at(dist(gen)));

    LOG_PRINT_L1("Pulling from the following nodes this LdPoW round.");
    LOG_PRINT_L1(two_nodes.at(0));
    LOG_PRINT_L1(two_nodes.at(1));

    /* Don't LdPoW before 1000th block */
    if (height > 800){
            std::vector<std::string> block_hashes;
            std::vector<std::string> block_hashes_reversed;
            uint64_t height_to_checkpoint = height - 20;
            std::string block_height_string = std::to_string(height_to_checkpoint);

            for (const auto &value: two_nodes){
                      std::string request_url_ldpow = value + "get_block/" + block_height_string;
                      std::string response_ldpow = "";
                      std::string log_message = "Getting block from " + value + " node now.";

                      bool ldpow_request = get_request_ldpow(request_url_ldpow, response_ldpow, log_message);
                      if(ldpow_request == true){
                              LOG_PRINT_L1("Request for node " << value << " went through.");
                        			std::stringstream string_stream(response_ldpow);
			                        ptree strTree;
			                        read_json(string_stream, strTree);
                              std::string hash_string = "hash";

                                    if(response_ldpow.find(hash_string) != std::string::npos){
                                            std::string block_hash = strTree.get<std::string> ("hash");
				                                    LOG_PRINT_L1("Request seems to have passed, " << block_hash << " recieved from LdPoW node "<< value <<" for block height : " << block_height_string << '\n');
				                                    block_hashes.push_back(block_hash);	// Push the block hash we got from LdPoW node into a list.
                                    }else{
      	                                    LOG_PRINT_L1("Block height of " << block_height_string << " not available with " << value);
				                                    return true;
		                                }
                      }else{
                        LOG_PRINT_L0("Something went wrong with LdPoW, skipping!");
                      }
            }
            int list_size = block_hashes.size();
            if (list_size < 2){
                /* This would never occur as the above loop will return true if there are nodes that doesn't reply to our requests*/
                LOG_PRINT_L0("No response from some of the lDPoW nodes, not adding any checkpoints now.");
                return true;
            }else{
                block_hashes_reversed = block_hashes;
                std::reverse(block_hashes_reversed.begin(), block_hashes_reversed.end());

                if (block_hashes == block_hashes_reversed){
                  MGINFO("LdPoW has reported the same hash for block " << block_height_string << " adding as checkpoint!");
                  ADD_CHECKPOINT(height_to_checkpoint, block_hashes.back());
                }else{
                  MERROR("Node have not reported the same hash for block " << block_height_string << " network split detected!");
                }
            }
            return true;
    }else{
      /* Not yet time for enabling LdPoW */
      return true;
    }
    return true;
  }
  //---------------------------------------------------------------------------
  bool checkpoints::load_checkpoints_from_dns(network_type nettype)
  {
    std::vector<std::string> records;

    // All four ScalaPulse domains have DNSSEC on and valid
    static const std::vector<std::string> dns_urls = { "checkpointsdns.scalaproject.io" };

    static const std::vector<std::string> testnet_dns_urls = { "testcheckpointsdns.scalaproject.io" };

    static const std::vector<std::string> stagenet_dns_urls = { "stagecheckpointsdns.scalaproject.io" };

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
