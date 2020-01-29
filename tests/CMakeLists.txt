#   Copyright 2020 Comcast Cable Communications Management, LLC
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -g -fprofile-arcs -ftest-coverage -O0")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTEST ")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W  -g -fprofile-arcs -ftest-coverage -O0")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -O0")

if(NOT DISABLE_VALGRIND)
set (MEMORY_CHECK valgrind --leak-check=full --show-reachable=yes -v)
endif ()

link_directories ( ${LIBRARY_DIR} )

#-------------------------------------------------------------------------------
#   test_dhcp
#-------------------------------------------------------------------------------
add_test(NAME test_dhcp COMMAND ${MEMORY_CHECK} ./test_dhcp)
add_executable(test_dhcp test_dhcp.c ../src/dhcp.c ../src/helpers.c)
target_link_libraries (test_dhcp -lcunit -lmsgpackc)

target_link_libraries (test_dhcp gcov -Wl,--no-as-needed )

#-------------------------------------------------------------------------------
#   test_envelope
#-------------------------------------------------------------------------------
add_test(NAME test_envelope COMMAND ${MEMORY_CHECK} ./test_envelope)
add_executable(test_envelope test_envelope.c ../src/envelope.c ../src/helpers.c)
target_link_libraries (test_envelope -lcunit -lmsgpackc)

target_link_libraries (test_envelope gcov -Wl,--no-as-needed )

#-------------------------------------------------------------------------------
#   test_firewall
#-------------------------------------------------------------------------------
add_test(NAME test_firewall COMMAND ${MEMORY_CHECK} ./test_firewall)
add_executable(test_firewall test_firewall.c ../src/firewall.c ../src/helpers.c)
target_link_libraries (test_firewall -lcunit -lmsgpackc)

target_link_libraries (test_firewall gcov -Wl,--no-as-needed )

#-------------------------------------------------------------------------------
#   test_full
#-------------------------------------------------------------------------------
add_test(NAME test_full COMMAND ${MEMORY_CHECK} ./test_full)
add_executable(test_full test_full.c ../src/full.c ../src/helpers.c)
target_link_libraries (test_full -lcunit -lmsgpackc)

target_link_libraries (test_full gcov -Wl,--no-as-needed )


#-------------------------------------------------------------------------------
#   test_gre
#-------------------------------------------------------------------------------
add_test(NAME test_gre COMMAND ${MEMORY_CHECK} ./test_gre)
add_executable(test_gre test_gre.c ../src/gre.c ../src/helpers.c)
target_link_libraries (test_gre -lcunit -lmsgpackc)

target_link_libraries (test_gre gcov -Wl,--no-as-needed )


#-------------------------------------------------------------------------------
#   test_http_headers
#-------------------------------------------------------------------------------
add_test(NAME test_http_headers COMMAND ${MEMORY_CHECK} ./test_http_headers)
add_executable(test_http_headers test_http_headers.c ../src/http_headers.c)
target_link_libraries (test_http_headers -lcunit )

target_link_libraries (test_http_headers gcov -Wl,--no-as-needed )



#-------------------------------------------------------------------------------
#   test_http
#-------------------------------------------------------------------------------
add_test(NAME test_http COMMAND ${MEMORY_CHECK} ./test_http)
add_executable(test_http test_http.c ../src/http.c ../src/http_headers.c)
target_link_libraries (test_http -lcunit -lcurl )

target_link_libraries (test_http gcov -Wl,--no-as-needed )

#-------------------------------------------------------------------------------
#   test_portmapping
#-------------------------------------------------------------------------------
add_test(NAME test_portmapping COMMAND ${MEMORY_CHECK} ./test_portmapping)
add_executable(test_portmapping test_portmapping.c ../src/portmapping.c ../src/helpers.c)
target_link_libraries (test_portmapping -lcunit -lmsgpackc)

target_link_libraries (test_portmapping gcov -Wl,--no-as-needed )

#-------------------------------------------------------------------------------
#   test_wifi
#-------------------------------------------------------------------------------
add_test(NAME test_wifi COMMAND ${MEMORY_CHECK} ./test_wifi)
add_executable(test_wifi test_wifi.c ../src/wifi.c ../src/helpers.c)
target_link_libraries (test_wifi -lcunit -lmsgpackc)

target_link_libraries (test_wifi gcov -Wl,--no-as-needed )

#-------------------------------------------------------------------------------
#   test_xdns
#-------------------------------------------------------------------------------
add_test(NAME test_xdns COMMAND ${MEMORY_CHECK} ./test_xdns)
add_executable(test_xdns test_xdns.c ../src/xdns.c ../src/helpers.c)
target_link_libraries (test_xdns -lcunit -lmsgpackc)

target_link_libraries (test_xdns gcov -Wl,--no-as-needed )


# Code coverage

add_custom_target(coverage
COMMAND lcov -q --capture --directory 
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_http_headers.dir/__/src --output-file test_http_headers.info
COMMAND lcov -q --capture --directory 
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_dhcp.dir/__/src --output-file test_dhcp.info
COMMAND lcov -q --capture --directory 
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_envelope.dir/__/src --output-file test_envelope.info
COMMAND lcov -q --capture --directory 
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_firewall.dir/__/src --output-file test_firewall.info
COMMAND lcov -q --capture --directory 
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_full.dir/__/src --output-file test_full.info
COMMAND lcov -q --capture --directory 
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_gre.dir/__/src --output-file test_gre.info
COMMAND lcov -q --capture --directory 
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_portmapping.dir/__/src --output-file test_portmapping.info
COMMAND lcov -q --capture --directory 
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_wifi.dir/__/src --output-file test_wifi.info
COMMAND lcov -q --capture --directory 
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_xdns.dir/__/src --output-file test_xdns.info

COMMAND lcov
-a test_http_headers.info
-a test_envelope.info
-a test_firewall.info
-a test_full.info
-a test_dhcp.info
-a test_gre.info
-a test_portmapping.info
-a test_wifi.info
-a test_xdns.info
--output-file coverage.info

COMMAND genhtml coverage.info
WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
