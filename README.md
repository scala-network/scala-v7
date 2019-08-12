
# Scala

Copyright (c) 2014-2019 The Monero Project

Copyright (c) 2017-2019, The Scala Project

Portions Copyright (c) 2012-2013, The Cryptonote developers

Portions Copyright (c) 2017 The Masari Project
  

## Table of Contents

-  [Development resources](#development-resources)

-  [Introduction](#introduction)

-  [About this project](#about-this-project)

-  [Supporting the project](#supporting-the-project)

-  [License](#license)

-  [Scheduled software upgrades](#scheduled-software-upgrades)

-  [Release staging schedule and protocol](#release-staging-schedule-and-protocol)

-  [Compiling Scala from source](#compiling-scala-from-source)

-  [Dependencies](#dependencies)

  
## Development resources

 
- Web: [scalaproject.io](https://scalaproject.io)

- Discord: [https://discord.gg/QfCWRfx](https://discord.gg/QfCWRfx)

- Mail: [hello@scalaproject.io](mailto:hello@scalaproject.io)

- GitHub: [https://github.com/scala-network/scala](https://github.com/scala-project/scala)


## Introduction

Scala is a private, secure, untraceable, decentralised digital currency. You are your bank, you control your funds, and nobody can trace your transfers unless you allow them to do so.

**Privacy:** Scala uses a cryptographically sound system to allow you to send and receive funds without your transactions being easily revealed on the blockchain (the ledger of transactions that everyone has). This ensures that your purchases, receipts, and all transfers remain absolutely private by default.


**Security:** Using the power of a distributed peer-to-peer consensus network, every transaction on the network is cryptographically secured. Individual wallets have a 25 word mnemonic seed that is only displayed once, and can be written down to backup the wallet. Wallet files are encrypted with a passphrase to ensure they are useless if stolen.


**Untraceability:** By taking advantage of ring signatures, a special property of a certain type of cryptography, Scala is able to ensure that transactions are not only untraceable, but have an optional measure of ambiguity that ensures that transactions cannot easily be tied back to an individual user or computer.

**Decentralization:** The utility of scala depends on its decentralised peer-to-peer consensus network - anyone should be able to run the scala software, validate the integrity of the blockchain, and participate in all aspects of the scala network using consumer-grade commodity hardware. Decentralization of the scala network is maintained by software development that minimizes the costs of running the scala software and inhibits the proliferation of specialized, non-commodity hardware.

**Egalitarian**: Scala has a very egalitarian proof-of-work allowing anyone with very simple hardware to participate in mining.

  
## About this project

This is the core implementation of Scala. It is open source and completely free to use without restrictions, except for those specified in the license agreement below. There are no restrictions on anyone creating an alternative implementation of Scala that uses the protocol and network in a compatible manner.

As with many development projects, the repository on Github is considered to be the "staging" area for the latest changes. Before changes are merged into that branch on the main repository, they are tested by individual developers in their own branches, submitted as a pull request, and then subsequently tested by contributors who focus on testing and code reviews. That having been said, the repository should be carefully considered before using it in a production environment, unless there is a patch in the repository for a particular show-stopping issue you are experiencing. It is generally a better idea to use a tagged release for stability.
  
**Anyone is welcome to contribute to Scala's codebase!** If you have a fix or code change, feel free to submit it as a pull request directly to the "master" branch. In cases where the change is relatively small or does not affect other parts of the codebase it may be merged in immediately by any one of the collaborators. On the other hand, if the change is particularly large or complex, it is expected that it will be discussed at length either well in advance of the pull request being submitted, or even directly on the pull request.

  
## Supporting the project

Scala is a 100% community-sponsored endeavor. If you want to join our efforts, the easiest thing you can do is support the project financially. Both Scala and Bitcoin donations can be made to addresses given below.

The Scala donation address is: `SEiTBcLGpfm3uj5b5RaZDGSUoAGnLCyG5aJjAwko67jqRwWEH26NFPd26EUpdL1zh4RTmTdRWLz8WCmk5F4umYaFByMtJT6RLjD6vzApQJWfi`

The Bitcoin donation address is: `1XTLY5LqdBXRW6hcHtnuMU7c68mAyW6qm`

If you would like to sponsor our project, please drop a twitter message at [ScalaHQ](https://twitter.com/ScalaHQ) or text us on [discord](https://discord.gg/QfCWRfx)

## License

See [LICENSE](LICENSE).


## Scheduled software upgrades


Scala uses a fixed-schedule software upgrade (hard fork) mechanism to implement new features. This means that users of Scala (end users and service providers) should run current versions and upgrade their software on a regular schedule.  The required software for these upgrades will be available prior to the scheduled date. Please check the repository prior to this date for the proper Scala software version. 


## Compiling Scala from source

### Dependencies

  
The following table summarizes the tools and libraries required to build. A

few of the libraries are also included in this repository (marked as

"Vendored"). By default, the build uses the library installed on the system,

and ignores the vendored sources. However, if no library is found installed on

the system, then the vendored source will be built and used. The vendored

sources are also used for statically-linked builds because distribution

packages often include only shared library binaries (`.so`) but not static

library archives (`.a`).

  

| Dep | Min. version | Vendored | Debian/Ubuntu pkg | Arch pkg | Fedora | Optional | Purpose |

| ------------ | ------------- | -------- | ------------------ | ------------ | ----------------- | -------- | -------------- |

| GCC | 4.7.3 | NO | `build-essential` | `base-devel` | `gcc` | NO | |

| CMake | 3.5 | NO | `cmake` | `cmake` | `cmake` | NO | |

| pkg-config | any | NO | `pkg-config` | `base-devel` | `pkgconf` | NO | |

| Boost | 1.58 | NO | `libboost-all-dev` | `boost` | `boost-devel` | NO | C++ libraries |

| OpenSSL | basically any | NO | `libssl-dev` | `openssl` | `openssl-devel` | NO | sha256 sum |

| libzmq | 3.0.0 | NO | `libzmq3-dev` | `zeromq` | `cppzmq-devel` | NO | ZeroMQ library |

| OpenPGM | ? | NO | `libpgm-dev` | `libpgm` | `openpgm-devel` | NO | For ZeroMQ |

| libnorm[2] | ? | NO | `libnorm-dev` | | ` | YES | For ZeroMQ |

| libunbound | 1.4.16 | YES | `libunbound-dev` | `unbound` | `unbound-devel` | NO | DNS resolver |

| libsodium | ? | NO | `libsodium-dev` | `libsodium` | `libsodium-devel` | NO | cryptography |

| libunwind | any | NO | `libunwind8-dev` | `libunwind` | `libunwind-devel` | YES | Stack traces |

| liblzma | any | NO | `liblzma-dev` | `xz` | `xz-devel` | YES | For libunwind |

| libreadline | 6.3.0 | NO | `libreadline6-dev` | `readline` | `readline-devel` | YES | Input editing |

| ldns | 1.6.17 | NO | `libldns-dev` | `ldns` | `ldns-devel` | YES | SSL toolkit |

| expat | 1.1 | NO | `libexpat1-dev` | `expat` | `expat-devel` | YES | XML parsing |

| GTest | 1.5 | YES | `libgtest-dev`[1] | `gtest` | `gtest-devel` | YES | Test suite |

| Doxygen | any | NO | `doxygen` | `doxygen` | `doxygen` | YES | Documentation |

| Graphviz | any | NO | `graphviz` | `graphviz` | `graphviz` | YES | Documentation |

  
  

[1] On Debian/Ubuntu `libgtest-dev` only includes sources and headers. You must

build the library binary manually. This can be done with the following command ```sudo apt-get install libgtest-dev && cd /usr/src/gtest && sudo cmake . && sudo make && sudo mv libg* /usr/lib/ ```

[2] libnorm-dev is needed if your zmq library was built with libnorm, and not needed otherwise

  

Install all dependencies at once on Debian/Ubuntu:

  

``` sudo apt update && sudo apt install build-essential cmake pkg-config libboost-all-dev libssl-dev libzmq3-dev libunbound-dev libsodium-dev libunwind8-dev liblzma-dev libreadline6-dev libldns-dev libexpat1-dev doxygen graphviz libpgm-dev```

  

Install all dependencies at once on macOS with the provided Brewfile:

``` brew update && brew bundle --file=contrib/brew/Brewfile ```

  

FreeBSD one liner for required to build dependencies

```pkg install git gmake cmake pkgconf boost-libs cppzmq libsodium```

  

### Cloning the repository

  

Clone recursively to pull-in needed submodule(s):

  

`$ git clone --recursive https://github.com/scala-project/scala`

  

If you already have a repo cloned, initialize and update:

  

`$ cd scala && git submodule init && git submodule update`

  

### Build instructions

  

Scala uses the CMake build system and a top-level [Makefile](Makefile) that

invokes cmake commands as needed.

  

#### On Linux and macOS

  

* Install the dependencies

* Change to the root of the source code directory, change to the most recent release branch, and build:

  

```bash
cd scala

make
```

*Optional*: If your machine has several cores and enough memory, enable

parallel build by running `make -j<number of threads>` instead of `make`. For

this to be worthwhile, the machine should have one core and about 2GB of RAM

available per thread.

  

*Note*: If cmake can not find zmq.hpp file on macOS, installing `zmq.hpp` from

https://github.com/zeromq/cppzmq to `/usr/local/include` should fix that error.

  

*Note*: The instructions above will compile the most stable release of the

Scala software. If you would like to use and test the most recent software,

use ```git checkout master```. The master branch may contain updates that are

both unstable and incompatible with release software, though testing is always

encouraged.

  

* The resulting executables can be found in `build/release/bin`

  

* Add `PATH="$PATH:$HOME/scala/build/release/bin"` to `.profile`

  

* Run Scala with `scalad --detach`

  

*  **Optional**: build and run the test suite to verify the binaries:

  

```bash

make release-test

```

  

*NOTE*: `core_tests` test may take a few hours to complete.

  

*  **Optional**: to build binaries suitable for debugging:

  

```bash

make debug

```

  

*  **Optional**: to build statically-linked binaries:

  

```bash

make release-static

```

  

Dependencies need to be built with -fPIC. Static libraries usually aren't, so you may have to build them yourself with -fPIC. Refer to their documentation for how to build them.

  

*  **Optional**: build documentation in `doc/html` (omit `HAVE_DOT=YES` if `graphviz` is not installed):

  

```bash

HAVE_DOT=YES doxygen Doxyfile

```

  

#### On the Raspberry Pi

Tested on a Raspberry Pi Zero with a clean install of minimal Raspbian Stretch (2017-09-07 or later) from https://www.raspberrypi.org/downloads/raspbian/. 


*  `apt-get update && apt-get upgrade` to install all of the latest software

  

* Install the dependencies for Scala from the 'Debian' column in the table above.

  

* Increase the system swap size:

  

```bash

sudo /etc/init.d/dphys-swapfile stop

sudo nano /etc/dphys-swapfile

CONF_SWAPSIZE=2048

sudo /etc/init.d/dphys-swapfile start

```

* If using an external hard disk without an external power supply, ensure it gets enough power to avoid hardware issues when syncing, by adding the line "max_usb_current=1" to /boot/config.txt

* Clone scala and checkout the most recent release version: 

```bash

git clone https://github.com/scala-project/scala.git

cd scala
```

* Build:

  

```bash

make release

```

* Wait 4-6 hours

* The resulting executables can be found in `build/release/bin`

* Add `PATH="$PATH:$HOME/scala/build/release/bin"` to `.profile`

* Run Scala with `scalad --detach`

* You may wish to reduce the size of the swap file after the build has finished, and delete the boost directory from your home directory
  

#### On Windows:

  

Binaries for Windows are built on Windows using the MinGW toolchain within

[MSYS2 environment](https://www.msys2.org). The MSYS2 environment emulates a

POSIX system. The toolchain runs within the environment and *cross-compiles*

binaries that can run outside of the environment as a regular Windows

application.

  

**Preparing the build environment**

  

* Download and install the [MSYS2 installer](https://www.msys2.org), either the 64-bit or the 32-bit package, depending on your system.

* Open the MSYS shell via the `MSYS2 Shell` shortcut

* Update packages using pacman:

  

```bash

pacman -Syu

```

  

* Exit the MSYS shell using Alt+F4

* Edit the properties for the `MSYS2 Shell` shortcut changing "msys2_shell.bat" to "msys2_shell.cmd -mingw64" for 64-bit builds or "msys2_shell.cmd -mingw32" for 32-bit builds

* Restart MSYS shell via modified shortcut and update packages again using pacman:

  

```bash

pacman -Syu

```

  
  

* Install dependencies:

  

To build for 64-bit Windows:

  

```bash

pacman -S mingw-w64-x86_64-toolchain make mingw-w64-x86_64-cmake mingw-w64-x86_64-boost mingw-w64-x86_64-openssl mingw-w64-x86_64-zeromq mingw-w64-x86_64-libsodium mingw-w64-x86_64-hidapi

```

  

To build for 32-bit Windows:

  

```bash

pacman -S mingw-w64-i686-toolchain make mingw-w64-i686-cmake mingw-w64-i686-boost mingw-w64-i686-openssl mingw-w64-i686-zeromq mingw-w64-i686-libsodium mingw-w64-i686-hidapi

```

  

* Open the MingW shell via `MinGW-w64-Win64 Shell` shortcut on 64-bit Windows

or `MinGW-w64-Win64 Shell` shortcut on 32-bit Windows. Note that if you are

running 64-bit Windows, you will have both 64-bit and 32-bit MinGW shells.

  

**Cloning**

  

* To git clone, run:

  

```bash

git clone https://github.com/scala-project/scala.git

```

**Building**

* Change to the cloned directory, run:

  

```bash

cd scala

```

  
* If you are on a 64-bit system, run:

  

```bash

make release-static-win64

```

  

* If you are on a 32-bit system, run:

  

```bash

make release-static-win32

```

  

* The resulting executables can be found in `build/release/bin`

  

*  **Optional**: to build Windows binaries suitable for debugging on a 64-bit system, run:

  

```bash

make debug-static-win64

```

  

*  **Optional**: to build Windows binaries suitable for debugging on a 32-bit system, run:

  

```bash

make debug-static-win32

```

  

* The resulting executables can be found in `build/debug/bin`


### Building portable statically linked binaries

By default, in either dynamically or statically linked builds, binaries target the specific host processor on which the build happens and are not portable to other processors. Portable binaries can be built using the following targets:

  

*  ```make release-static-linux-x86_64``` builds binaries on Linux on x86_64 portable across POSIX systems on x86_64 processors

*  ```make release-static-linux-i686``` builds binaries on Linux on x86_64 or i686 portable across POSIX systems on i686 processors

*  ```make release-static-linux-armv8``` builds binaries on Linux portable across POSIX systems on armv8 processors

*  ```make release-static-linux-armv7``` builds binaries on Linux portable across POSIX systems on armv7 processors

*  ```make release-static-linux-armv6``` builds binaries on Linux portable across POSIX systems on armv6 processors

*  ```make release-static-win64``` builds binaries on 64-bit Windows portable across 64-bit Windows systems

*  ```make release-static-win32``` builds binaries on 64-bit or 32-bit Windows portable across 32-bit Windows systems

  

### Cross Compiling

  
You can also cross-compile static binaries on Linux for Windows and macOS with the `depends` system.

  
*  ```make depends target=x86_64-linux-gnu``` for 64-bit linux binaries.

*  ```make depends target=x86_64-w64-mingw32``` for 64-bit windows binaries.

* Requires: `python3 g++-mingw-w64-x86-64 wine1.6 bc`

*  ```make depends target=x86_64-apple-darwin11``` for macOS binaries.

* Requires: `cmake imagemagick libcap-dev librsvg2-bin libz-dev libbz2-dev libtiff-tools python-dev`

*  ```make depends target=i686-linux-gnu``` for 32-bit linux binaries.

* Requires: `g++-multilib bc`

*  ```make depends target=i686-w64-mingw32``` for 32-bit windows binaries.

* Requires: `python3 g++-mingw-w64-i686`

*  ```make depends target=arm-linux-gnueabihf``` for armv7 binaries.

* Requires: `g++-arm-linux-gnueabihf`

*  ```make depends target=aarch64-linux-gnu``` for armv8 binaries.

* Requires: `g++-aarch64-linux-gnu`

  

The required packages are the names for each toolchain on apt. Depending on your distro, they may have different names.

  

Using `depends` might also be easier to compile Scala on Windows than using MSYS. Activate Windows Subsystem for Linux (WSL) with a distro (for example Ubuntu), install the apt build-essentials and follow the `depends` steps as depicted above.

  

The produced binaries still link libc dynamically. If the binary is compiled on a current distribution, it might not run on an older distribution with an older installation of libc. Passing `-DBACKCOMPAT=ON` to cmake will make sure that the binary will run on systems having at least libc version 2.17.
  

## Running scalad

  

The build places the binary in `bin/` sub-directory within the build directory

from which cmake was invoked (repository root by default). To run in

foreground:

```bash

./bin/scalad

```

To list all available options, run `./bin/scalad --help`. Options can be

specified either on the command line or in a configuration file passed by the

`--config-file` argument. To specify an option in the configuration file, add

a line with the syntax `argumentname=value`, where `argumentname` is the name

of the argument without the leading dashes, for example `log-level=1`.

To run in background:

```bash

./bin/scalad --log-file scalad.log --detach

```

To run as a systemd service, copy

[scalad.service](utils/systemd/scalad.service) to `/etc/systemd/system/` and

[scalad.conf](utils/conf/scalad.conf) to `/etc/`. The [example

service](utils/systemd/scalad.service) assumes that the user `scala` exists

and its home is the data directory specified in the [example

config](utils/conf/scalad.conf).

  

If you're on Mac, you may need to add the `--max-concurrency 1` option to

scala-wallet-cli, and possibly scalad, if you get crashes refreshing.

  

## Debugging

  

This section contains general instructions for debugging failed installs or problems encountered with Scala. First, ensure you are running the latest version built from the Github repo.

  

### Obtaining stack traces and core dumps on Unix systems

  

We generally use the tool `gdb` (GNU debugger) to provide stack trace functionality, and `ulimit` to provide core dumps in builds which crash or segfault.

  

* To use `gdb` in order to obtain a stack trace for a build that has stalled:

  

Run the build.

  

Once it stalls, enter the following command:

  

```bash

gdb /path/to/scalad `pidof scalad`

```

  

Type `thread apply all bt` within gdb in order to obtain the stack trace

  

* If however the core dumps or segfaults:

  

Enter `ulimit -c unlimited` on the command line to enable unlimited filesizes for core dumps

  

Enter `echo core | sudo tee /proc/sys/kernel/core_pattern` to stop cores from being hijacked by other tools

  

Run the build.

  

When it terminates with an output along the lines of "Segmentation fault (core dumped)", there should be a core dump file in the same directory as scalad. It may be named just `core`, or `core.xxxx` with numbers appended.

  

You can now analyse this core dump with `gdb` as follows:

  

```bash

gdb /path/to/scalad /path/to/dumpfile`

```

  

Print the stack trace with `bt`

  

#### To run scala within gdb:

  

Type `gdb /path/to/scalad`

  

Pass command-line options with `--args` followed by the relevant arguments

  

Type `run` to run scalad

  

### Analysing memory corruption

  

There are two tools available:

  

#### ASAN

  

Configure Scala with the -D SANITIZE=ON cmake flag, eg:

  

```bash

cd build/debug && cmake -D SANITIZE=ON -D CMAKE_BUILD_TYPE=Debug ../..

```

  

You can then run the scala tools normally. Performance will typically halve.

  

#### valgrind

  

Install valgrind and run as `valgrind /path/to/scalad`. It will be very slow.

  

### LMDB

  

Instructions for debugging suspected blockchain corruption as per @HYC

  

There is an `mdb_stat` command in the LMDB source that can print statistics about the database but it's not routinely built. This can be built with the following command:

  

```bash

cd  ~/scala/external/db_drivers/liblmdb && make

```

  

The output of `mdb_stat -ea <path to blockchain dir>` will indicate inconsistencies in the blocks, block_heights and block_info table.

  

The output of `mdb_dump -s blocks <path to blockchain dir>` and `mdb_dump -s block_info <path to blockchain dir>` is useful for indicating whether blocks and block_info contain the same keys.

  

These records are dumped as hex data, where the first line is the key and the second line is the data.

