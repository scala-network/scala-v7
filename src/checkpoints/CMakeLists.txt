# Copyright (c) 2014-2019, The Monero Project
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of
#    conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list
#    of conditions and the following disclaimer in the documentation and/or other
#    materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be
#    used to endorse or promote products derived from this software without specific
#    prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

if(APPLE)
  if(DEPENDS)
    list(APPEND EXTRA_LIBRARIES "-framework Foundation -framework ApplicationServices -framework AppKit -framework IOKit")
  else()
    find_library(IOKIT_LIBRARY IOKit)
    mark_as_advanced(IOKIT_LIBRARY)
    list(APPEND EXTRA_LIBRARIES ${IOKIT_LIBRARY})
  endif()
endif()

message(${CMAKE_CURRENT_SOURCE_DIR})


set(checkpoints_sources
  checkpoints.cpp)

if(UNIX)
	set(checkpoints_headers
    	./libznipfs/json.hpp
    	./libznipfs/HTTPRequest.hpp
    	./libznipfs/libznipfs-linux.h)
elseif(APPLE)
	set(checkpoints_headers
        ./libznipfs/json.hpp
        ./libznipfs/HTTPRequest.hpp
        ./libznipfs/libznipfs-windows.h)
else()
	set(checkpoints_headers
        ./libznipfs/json.hpp
        ./libznipfs/HTTPRequest.hpp
        ./libznipfs/libznipfs-mac.h)
endif()



set(checkpoints_private_headers
  checkpoints.h)

scala_private_headers(checkpoints
  ${checkpoints_private_headers})
scala_add_library(checkpoints
  ${checkpoints_sources}
  ${checkpoints_headers}
  ${checkpoints_private_headers})
if(UNIX)
    target_link_libraries(checkpoints
    PUBLIC
        common
        cncrypto
        ${Boost_DATE_TIME_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_SERIALIZATION_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${CMAKE_CURRENT_SOURCE_DIR}/libznipfs/libznipfs-linux.a
    PRIVATE
        ${EXTRA_LIBRARIES})
elseif(APPLE)
    target_link_libraries(checkpoints
    PUBLIC
        common
        cncrypto
        ${Boost_DATE_TIME_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_SERIALIZATION_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${CMAKE_CURRENT_SOURCE_DIR}/libznipfs/libznipfs-macos.a
    PRIVATE
        ${EXTRA_LIBRARIES})
else()
    target_link_libraries(checkpoints
    PUBLIC
        common
        cncrypto
        ${Boost_DATE_TIME_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_SERIALIZATION_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${CMAKE_CURRENT_SOURCE_DIR}/libznipfs/libznipfs-windows.a
    PRIVATE
        ${EXTRA_LIBRARIES})
endif()
