set -e
set -x
##############################
#      Setup WorkSpace       #
##############################
GITHUB_WORKSPACE="${PWD}"
ls -la ${GITHUB_WORKSPACE}
cd ${GITHUB_WORKSPACE}

##############################
#        Build WEBCFG        #
##############################
echo "======================================================================================"
echo "buliding webcfg for coverity"

cd ${GITHUB_WORKSPACE}
cmake -S "$GITHUB_WORKSPACE" -B build -DWEBCONFIG_BIN_SUPPORT:BOOL=true -DWAN_FAILOVER_SUPPORTED:BOOL=true

cmake --build build --target install
echo "======================================================================================"
exit 0
