#!/bin/bash
##########################################################################
# If not stated otherwise in this file or this component's LICENSE
# file the following copyright and licenses apply:
#
# Copyright 2019 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
#######################################
#
# Build Framework standard script for
#
#
# use -e to fail on any shell issue
# -e is the requirement from Build Framework
set -e

# default PATHs - use `man readlink` for more info
# the path to combined build
export RDK_PROJECT_ROOT_PATH=${RDK_PROJECT_ROOT_PATH-`readlink -m ..`}
export COMBINED_ROOT=$RDK_PROJECT_ROOT_PATH

# path to build script (this script)
export RDK_SCRIPTS_PATH=${RDK_SCRIPTS_PATH-`readlink -m $0 | xargs dirname`}
export RDK_TOOLCHAIN_PATH=${RDK_TOOLCHAIN_PATH-`readlink -m $RDK_PROJECT_ROOT_PATH/sdk/toolchain/staging_dir`}

# path to components sources and target
export RDK_SOURCE_PATH=${RDK_SOURCE_PATH-`readlink -m .`}
export RDK_TARGET_PATH=${RDK_TARGET_PATH-$RDK_SOURCE_PATH}

# default component name
export RDK_COMPONENT_NAME=${RDK_COMPONENT_NAME-`basename $RDK_SOURCE_PATH`}
export RDK_DIR=$RDK_PROJECT_ROOT_PATH
export RDK_FSROOT_PATH=$RDK_PROJECT_ROOT_PATH/sdk/fsroot/ramdisk

if [ "$XCAM_MODEL" == "SCHC2" ]; then
RDK_PATCHES=$RDK_PROJECT_ROOT_PATH/build/components/amba/opensource/patch
else
RDK_PATCHES=$RDK_PROJECT_ROOT_PATH/build/components/opensource/patch
fi
#source $RDK_SCRIPTS_PATH/soc/build/soc_env.sh

# To enable rtMessage
export RTMESSAGE=yes

if [ "$XCAM_MODEL" == "SCHC2" ]; then
 echo "Configuring for XCAM2"
 source  ${RDK_PROJECT_ROOT_PATH}/build/components/amba/sdk/setenv2
elif [ "$XCAM_MODEL" == "SERXW3" ] || [ "$XCAM_MODEL" == "SERICAM2" ] || [ "$XCAM_MODEL" == "XHB1" ] || [ "$XCAM_MODEL" == "XHC3" ]; then
 echo "Configuring for Other device Model"
  source ${RDK_PROJECT_ROOT_PATH}/build/components/sdk/setenv2
else #No Matching platform
    echo "Source environment that include packages for your platform. The environment variables PROJ_PRERULE_MAK_FILE should refer to the platform s PreRule make"
fi

# parse arguments
INITIAL_ARGS=$@
function usage()
{
    set +x
    echo "Usage: `basename $0` [-h|--help] [-v|--verbose] [action]"
    echo "    -h    --help                  : this help"
    echo "    -v    --verbose               : verbose output"
    echo "    -p    --platform  =PLATFORM   : specify platform for xw4 common"
    echo
    echo "Supported actions:"
    echo "      configure, clean, build (DEFAULT), rebuild, install"
}
# options may be followed by one colon to indicate they have a required argument
if ! GETOPT=$(getopt -n "build.sh" -o hvp: -l help,verbose,platform: -- "$@")
then
    usage
    exit 1
fi
eval set -- "$GETOPT"
while true; do
  case "$1" in
    -h | --help ) usage; exit 0 ;;
    -v | --verbose ) set -x ;;
    -p | --platform ) CC_PLATFORM="$2" ; shift ;;
    -- ) shift; break;;
    * ) break;;
  esac
  shift
done
ARGS=$@
# component-specific vars
if [ "$XCAM_MODEL" == "SCHC2" ]; then
	export CROSS_COMPILE=$RDK_TOOLCHAIN_PATH/bin/arm-linux-gnueabihf-
  export CC=${CROSS_COMPILE}gcc
  export CXX=${CROSS_COMPILE}g++
  export DEFAULT_HOST=arm-linux
  export PKG_CONFIG_PATH="$RDK_PROJECT_ROOT_PATH/opensource/lib/pkgconfig/:$RDK_FSROOT_PATH/img/fs/shadow_root/usr/local/lib/pkgconfig/:$RDK_TOOLCHAIN_PATH/lib/pkgconfig/:$PKG_CONFIG_PATH"
fi
PREFIX_FOLDER=`shopt -s extglob; echo ${RDK_TARGET_PATH%%+(/)}`

# functional modules
function configure()
{
    echo "Executing configure common code"
    cd $RDK_SOURCE_PATH

    if [ $XCAM_MODEL == "SCHC2" ]; then
      export LDFLAGS+="-L${RDK_PROJECT_ROOT_PATH}/opensource/lib -lmsgpackc -lcjson -lcurl -lcimplog -ltrower-base64 -lwdmp-c -lwrp-c -llinenoise -L${RDK_PROJECT_ROOT_PATH}/rdklogger/src/.libs -lrdkloggers -L${RDK_PROJECT_ROOT_PATH}/opensource/src/rtmessage/ -lrtMessage -Wl,-rpath-link=${RDK_PROJECT_ROOT_PATH}/libexchanger/Release/src -Wl,-rpath-link=${RDK_PROJECT_ROOT_PATH}/libexchanger/password/src -L${RDK_PROJECT_ROOT_PATH}/libsyswrapper/source/.libs/  -lsecure_wrapper -L${RDK_PROJECT_ROOT_PATH}/rbuscore/build/rbus-core/lib/ -lrbus-core -L${RDK_PROJECT_ROOT_PATH}/rbus/build/src/ -lrbus -L${RDK_PROJECT_ROOT_PATH}/webcfg-cpeabs/build/src/ -lcpeabs -Wl,-rpath-link=${RDK_PROJECT_ROOT_PATH}/opensource/lib"
    else
      export LDFLAGS+="-L${RDK_PROJECT_ROOT_PATH}/opensource/lib -lmsgpackc -lcjson -lcurl -lcimplog -ltrower-base64 -lwdmp-c -lwrp-c -llinenoise -L${RDK_PROJECT_ROOT_PATH}/rdklogger/src/.libs -lrdkloggers -L${RDK_PROJECT_ROOT_PATH}/opensource/src/rtmessage/ -lrtMessage -L${RDK_PROJECT_ROOT_PATH}/libsyswrapper/source/.libs/ -lsecure_wrapper -L${RDK_PROJECT_ROOT_PATH}/rbuscore/build/rbus-core/lib/ -lrbus-core -L${RDK_PROJECT_ROOT_PATH}/rbus/build/src/ -lrbus -L${RDK_PROJECT_ROOT_PATH}/webcfg-cpeabs/build/src/ -lcpeabs -Wl,-rpath-link=${RDK_PROJECT_ROOT_PATH}/opensource/lib"
    fi
    export CFLAGS+="-O2 -g -Wall -I$PREFIX_FOLDER/include -I${RDK_PROJECT_ROOT_PATH}/opensource/include -fPIC  -I${RDK_PROJECT_ROOT_PATH}/opensource/include/glib-2.0/ -I${RDK_PROJECT_ROOT_PATH}/opensource/lib/glib-2.0/include/ -I${RDK_PROJECT_ROOT_PATH}/opensource/include/glib-2.0/glib -I${RDK_PROJECT_ROOT_PATH}/opensource/include/cjson/ -I${RDK_PROJECT_ROOT_PATH}/opensource/include/cimplog/ -I${RDK_PROJECT_ROOT_PATH}/opensource/include/curl/ -I${RDK_PROJECT_ROOT_PATH}/opensource/include/trower-base64/ -I${RDK_PROJECT_ROOT_PATH}/opensource/include/wdmp-c/ -I${RDK_PROJECT_ROOT_PATH}/opensource/include/wrp-c/ -I${RDK_PROJECT_ROOT_PATH}/opensource/src/rtmessage -I${RDK_PROJECT_ROOT_PATH}/rbuscore/rbus-core/include/ -I${RDK_PROJECT_ROOT_PATH}/rbuscore/rbus-core/session_manager/ -I${RDK_PROJECT_ROOT_PATH}/rbus/include/ -I${RDK_PROJECT_ROOT_PATH}/rdklogger/include -I${RDK_PROJECT_ROOT_PATH}/libsyswrapper/source/ -I${RDK_FSROOT_PATH}/usr/include/ -I${RDK_FSROOT_PATH}/usr/include/ -DENABLE_RDKC_SUPPORT"
    export ac_cv_func_malloc_0_nonnull=yes
    export ac_cv_func_memset=yes
    
    cmake -DWEBCONFIG_BIN_SUPPORT=true -DBUILD_YOCTO=true -DBUILD_TESTING=OFF -DBUILD_CAMERA=true
}

function clean()
{
    echo "Start Clean"
    cd ${RDK_SOURCE_PATH}
    if [ -f Makefile ]; then
      make clean
    fi
    rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake test_results
}

function build()
{
   echo "Building webconfig client code"
   cd ${RDK_SOURCE_PATH}
   make
   install
}

function rebuild()
{
    clean
    configure
    build
}

function install()
{
    cd ${RDK_SOURCE_PATH}/src
    cp webconfig ${RDK_FSROOT_PATH}/usr/bin/
}

# run the logic
#these args are what left untouched after parse_args
HIT=false
for i in "$@"; do
    case $i in
        configure)  HIT=true; configure ;;
        clean)      HIT=true; clean ;;
        build)      HIT=true; build ;;
        rebuild)    HIT=true; rebuild ;;
        install)    HIT=true; install ;;
        *)
            #skip unknown
        ;;
    esac
done
# if not HIT do build by default
if ! $HIT; then
  build
fi
