# Stellite

Copyright (c) 2017, The Stellite Project

Copyright (c) 2017, The Electroneum Project

Copyright (c) 2014-2017, The Monero Project

Portions Copyright (c) 2012-2013, The Cryptonote developers

Portions Copyright (c) 2017 The Masari Project


## Development Resources

- Web: [stellite.cash](https://stellite.cash)
- Mail: [stellite@protonmail.com](mailto:stellite@protonmail.com)
- GitHub: [https://github.com/Stellitecoin](https://github.com/Stellitecoin)
- Blockexplorer: [explorer.stellite.cash](https://explorer.stellite.cash)

## Introduction

Stellite is a private, secure, untraceable cryptocurrency.We bring the best from the whole cryptonight space, both from Monero and Electroneum. In Stellite You are your bank, you control your funds, and nobody can trace your transfers unless you allow them to do so. Stellite is a soft fork of monero trying to implement the best of proposals in the entire privacy coin ecosystem.

**Features:** We are trying to do things no cryptocurrency has ever done before. Full blown small device mining for instance smartphones, tablets, smart tv etc etc are in our scopes. P2P distributed seed lists are the next target after that. All this coupled with the below given features will make stellite one of the best cryptocurrencies on the market.

**Privacy:** Stellite uses a cryptographically sound system to allow you to send and receive funds without your transactions being easily revealed on the blockchain (the ledger of transactions that everyone has). This ensures that your purchases, receipts, and all transfers remain absolutely private by default.

**Security:** Using the power of a distributed peer-to-peer consensus network, every transaction on the network is cryptographically secured. Individual wallets have a 25 word mnemonic seed that is only displayed once, and can be written down to backup the wallet. Wallet files are encrypted with a passphrase to ensure they are useless if stolen.

**Untraceability:** By taking advantage of ring signatures, a special property of a certain type of cryptography, Stellite is able to ensure that transactions are not only untraceable, but have an optional measure of ambiguity that ensures that transactions cannot easily be tied back to an individual user or computer.

**Timeline:** We are going to be implementing true small hashrate device mining and P2P distributed seed lists without hardcode by Q2 of 2018.

## About this Project

This is the core implementation of Stellite. It is open source and completely free to use without restrictions, except for those specified in the license agreement below. There are no restrictions on anyone creating an alternative implementation of Stellite that uses the protocol and network in a compatible manner.

As with many development projects, the repository on Github is considered to be the "staging" area for the latest changes. Before changes are merged into that branch on the main repository, they are tested by individual developers in their own branches, submitted as a pull request, and then subsequently tested by contributors who focus on testing and code reviews. That having been said, the repository should be carefully considered before using it in a production environment, unless there is a patch in the repository for a particular show-stopping issue you are experiencing. It is generally a better idea to use a tagged release for stability.

**Anyone is welcome to contribute to Stellite's codebase!** If you have a fix or code change, feel free to submit it as a pull request directly to the "master" branch. In cases where the change is relatively small or does not affect other parts of the codebase it may be merged in immediately by any one of the collaborators. On the other hand, if the change is particularly large or complex, it is expected that it will be discussed at length either well in advance of the pull request being submitted, or even directly on the pull request.

## License

See [LICENSE](LICENSE).

## Compiling Stellite from Source

### Dependencies

The following table summarizes the tools and libraries required to build.  A
few of the libraries are also included in this repository (marked as
"Vendored"). By default, the build uses the library installed on the system,
and ignores the vendored sources. However, if no library is found installed on
the system, then the vendored source will be built and used. The vendored
sources are also used for statically-linked builds because distribution
packages often include only shared library binaries (`.so`) but not static
library archives (`.a`).

| Dep            | Min. Version  | Vendored | Debian/Ubuntu Pkg  | Arch Pkg                    | Optional | Purpose        |
| -------------- | ------------- | ---------| ------------------ | --------------------------- | -------- | -------------- |
| GCC            | 4.7.3         | NO       | `build-essential`  | `base-devel`                | NO       |                |
| CMake          | 3.0.0         | NO       | `cmake`            | `cmake`                     | NO       |                |
| pkg-config     | any           | NO       | `pkg-config`       | `base-devel`                | NO       |                |
| Boost          | 1.58          | NO       | `libboost-all-dev` | `boost`                     | NO       | C++ libraries  |
| OpenSSL        | basically any | NO       | `libssl-dev`       | `openssl`                   | NO       | sha256 sum     |
| libunbound     | 1.4.16        | YES      | `libunbound-dev`   | `unbound`                   | NO       | DNS resolver   |
| libsodium      | ?             | NO       | `libsodium-dev`    | `libsodium-devel`           | NO       | libsodium      |
| libzmq         | 3.0.0         | NO       | `libzmq3-dev`      | `zeromq`                    | NO       | ZeroMQ library |
| libminiupnpc   | 2.0           | YES      | `libminiupnpc-dev` | `miniupnpc`                 | YES      | NAT punching   |
| libunwind      | any           | NO       | `libunwind8-dev`   | `libunwind`                 | YES      | Stack traces   |
| liblzma        | any           | NO       | `liblzma-dev`      | `xz`                        | YES      | For libunwind  |
| ldns           | 1.6.17        | NO       | `libldns-dev`      | `ldns`                      | YES      | SSL toolkit    |
| expat          | 1.1           | NO       | `libexpat1-dev`    | `expat`                     | YES      | XML parsing    |
| GTest          | 1.5           | YES      | `libgtest-dev`^    | `gtest`                     | YES      | Test suite     |
| Doxygen        | any           | NO       | `doxygen`          | `doxygen`                   | YES      | Documentation  |
| Graphviz       | any           | NO       | `graphviz`         | `graphviz`                  | YES      | Documentation  |
| pcsclite       | ?             | NO       | `libpcsclite-dev`  | `pcsc-lite pcsc-lite-devel` | NO       | Ledger         |

Debian / Ubuntu one liner for all dependencies  
``` sudo apt update && sudo apt install build-essential cmake pkg-config libboost-all-dev libssl-dev libzmq3-dev libunbound-dev libsodium-dev libminiupnpc-dev miniupnpc libunwind8-dev liblzma-dev libldns-dev libexpat1-dev doxygen graphviz libpcsclite-dev ```

[^] On Debian/Ubuntu `libgtest-dev` only includes sources and headers. You must
build the library binary manually. This can be done with the following command ```sudo apt-get install libgtest-dev && cd /usr/src/gtest && sudo cmake . && sudo make && sudo mv libg* /usr/lib/ ```

### Build instructions

Stellite uses the CMake build system and a top-level [Makefile](Makefile) that
invokes cmake commands as needed.

* Install the dependencies

## On Linux:

(Tested on Ubuntu 18.04 LTS x64, 16.04 x86, 16.10 x64, Gentoo x64 and Linux Mint 18 "Sarah" - Cinnamon x64)

1. Install Stellite dependencies

  - For Ubuntu and Mint

	`sudo apt install build-essential cmake libboost-all-dev miniupnpc libpcsclite-dev libzmq3-dev libminiupnpc-dev libunbound-dev graphviz doxygen libunwind8-dev pkg-config libssl-dev`

  - For Gentoo

	`sudo emerge app-arch/xz-utils app-doc/doxygen dev-cpp/gtest dev-libs/boost dev-libs/expat dev-libs/openssl dev-util/cmake media-gfx/graphviz net-dns/unbound net-libs/ldns net-libs/miniupnpc net-libs/zeromq sys-libs/libunwind`
        
## On OS X:

1. Install or update Xcode from the AppStore
2. Install [homebrew](http://brew.sh/) via the Terminal
3. Install [stellite](https://github.com/stellitecoin/stellite) dependencies via the Terminal:

  `brew install git`
  
  `brew install boost --c++11`

  `brew install openssl` - to install openssl headers

  `brew install pkgconfig`

  `brew install cmake`
  
  `brew install unbound`

  `brew install miniupnpc`

  `brew install --with-pgm --devel zeromq`

  `brew install doxygen`

  `brew install pcsc-lite`

### On Linux and OS X:

* Download the source code via Git:

`git clone --recursive https://github.com/stellitecoin/Stellite.git`

* Change to the root of the source code directory and build:

        cd Stellite
        make release-static

    *Optional*: If your machine has several cores and enough memory, enable
    parallel build by running `make -j<number of threads>` instead of `make`. For
    this to be worthwhile, the machine should have one core and about 2GB of RAM
    available per thread.

* The resulting executables can be found in `build/release/bin`

* Add `PATH="$PATH:$HOME/Stellite/build/release/bin"` to `.profile`

* Run Stellite with `stellited --detach`

* For wicked fast sync use `./stellited --add-exclusive-node [IP:PORT]` with one of these address and ports:

* 54.38.44.3:20188
* 85.25.35.35:20188
* 107.191.63.92:20188
* 172.110.10.6:20188


* **Optional**: build and run the test suite to verify the binaries:

        make release-test

    *NOTE*: `coretests` test may take a few hours to complete.

* **Optional**: to build binaries suitable for debugging:

         make debug

* **Optional**: to build statically-linked binaries:

         make release-static

* **Optional**: build documentation in `doc/html` (omit `HAVE_DOT=YES` if `graphviz` is not installed):

        HAVE_DOT=YES doxygen Doxyfile

#### On Windows:

Binaries for Windows are built on Windows using the MinGW toolchain within
[MSYS2 environment](http://msys2.github.io). The MSYS2 environment emulates a
POSIX system. The toolchain runs within the environment and *cross-compiles*
binaries that can run outside of the environment as a regular Windows
application.

**Preparing the Build Environment**

* Download and install the [MSYS2 installer](http://msys2.github.io), 64-bit package, depending on your system.
* Open the MSYS shell via the `MSYS2 Shell` shortcut
* Update packages using pacman:  

        pacman -Syuu  

* Exit the MSYS shell using Alt+F4  
* Edit the properties for the `MSYS2 Shell` shortcut changing "msys2_shell.bat" to "msys2_shell.cmd -mingw64"
* Restart MSYS shell via modified shortcut and update packages again using pacman:  

        pacman -Syuu  


* Install dependencies:

    To build for 64-bit Windows:

        pacman -S mingw-w64-x86_64-toolchain make mingw-w64-x86_64-cmake mingw-w64-x86_64-boost mingw-w64-x86_64-openssl mingw-w64-x86_64-zeromq mingw-w64-x86_64-libsodium


* Open the MingW shell via `MinGW-w64-Win64 Shell` shortcut on 64-bit Windows.
  
**Cloning**

* To git clone, run:

        git clone --recursive https://github.com/stellitecoin/Stellite.git

**Building**

* If you are on a 64-bit system, run:

        make release-static-win64

* The resulting executables can be found in `build/release/bin`

### Building Portable Statically Linked Binaries

By default, in either dynamically or statically linked builds, binaries target the specific host processor on which the build happens and are not portable to other processors. Portable binaries can be built using the following targets:

* ```make release-static-linux-x86_64``` builds binaries on Linux on x86_64 portable across POSIX systems on x86_64 processors
* ```make release-static-armv8``` builds binaries on Linux portable across POSIX systems on armv8 processors
* ```make release-static-armv7``` builds binaries on Linux portable across POSIX systems on armv7 processors
* ```make release-static-armv6``` builds binaries on Linux portable across POSIX systems on armv6 processors
* ```make release-static-win64``` builds binaries on 64-bit Windows portable across 64-bit Windows systems
* ```make release-static-win32``` builds binaries on 64-bit or 32-bit Windows portable across 32-bit Windows systems

## Running Stellited

The build places the binary in `bin/` sub-directory within the build directory
from which cmake was invoked (repository root by default). To run in
foreground:

    ./bin/Stellited

To list all available options, run `./bin/Stellited --help`.  Options can be
specified either on the command line or in a configuration file passed by the
`--config-file` argument.  To specify an option in the configuration file, add
a line with the syntax `argumentname=value`, where `argumentname` is the name
of the argument without the leading dashes, for example `log-level=1`.

To run in background:

    ./bin/Stellited --log-file Stellited.log --detach

If you're on Mac, you may need to add the `--max-concurrency 1` option to
Stellite-wallet-cli, and possibly Stellited, if you get crashes refreshing.
