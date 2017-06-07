#!/bin/bash -x
##############################################################################
# RunProcessor.sh
# Run a processing task names {procElem} with an input cfg {cfgFile}
#-----------------------------------------------------------------------------
# Usage:
#   RunProcessor.sh {procElem} {cfgFile}
##############################################################################

##=== Check if script call has enough arguments to continue ==================

usage() {
    echo "Usage: ${SCRIPT_NAME} {procElem} {cfgFile}"
}

if [ $# -lt 2 ]; then
    usage
    exit 1
fi

##=== Define environment =====================================================

#-- This script path and name ----------------------------------------
SCRIPT_PATH="${BASH_SOURCE[0]}";
SCRIPT_NAME=$(basename "${SCRIPT_PATH}")
if [ -h "${SCRIPT_PATH}" ]; then
    while [ -h "${SCRIPT_PATH}" ]; do
        SCRIPT_PATH=$(readlink "${SCRIPT_PATH}")
    done
fi
pushd . > /dev/null
cd $(dirname ${SCRIPT_PATH}) > /dev/null
SCRIPT_PATH=$(pwd)
popd  > /dev/null

#-- Basic variables used along this script ---------------------------

# Framework related variables
QPF_SYS_ROOT="${HOME}/qpf"
QPF_RUN="${QPF_SYS_ROOT}/run"

# Local framework installation related variables
DOCKER_TOOLS_PATH="${QPF_RUN}/bin"
PROC_BIN_ROOT="${QPF_SYS_ROOT}/bin"

# Image/Container related variables
DOCKER_IMAGE="qpf-debian:a"
QPF_IMG_RUN="/qpf/run"
QPF_IMG_BIN="/qlabin"

SUDO=""

##=== Prepare docker image invocation ========================================

#-- Catch command line arguments
procElem=$1
cfgFile=$2

#-- Get task execution path and translate to container FS
LOCAL_PATH=$(dirname $cfgFile)
IMG_PATH=${QPF_IMG_RUN}/$(basename $LOCAL_PATH)
BIN_PATH=${QPF_IMG_BIN}

#-- Create output and log folders, and link to processor
cd ${DOCKER_TOOLS_PATH}
rm -rf ${LOCAL_PATH}/out ${LOCAL_PATH}/log
mkdir -p ${LOCAL_PATH}/out ${LOCAL_PATH}/log
ln ${DOCKER_TOOLS_PATH}/${procElem}/${procElem} ${LOCAL_PATH}/
chown -hR --reference=${cfgFile} ${DOCKER_TOOLS_PATH}

#-- Complete definition of variables to use, depending on the
source ${DOCKER_TOOLS_PATH}/${procElem}/${procElem}.cfg

#-- Prepare Host-Container FS mapping
declare -A map
map[1]="${LOCAL_PATH}:${IMG_PATH}"
map[2]="${LOCAL_PATH}/in:${IMG_PATH}/in"
map[3]="${LOCAL_PATH}/out:${IMG_PATH}/out"
if [ -n "${BIN_PATH}" ]; then
    map[4]="${LOCAL_PROC_PATH}:${BIN_PATH}"
fi
MAPPED_DIRS=$(echo " ${map[@]}" |sed 's/ / -v /g')

#-- Define Docker call options
#OPTS="-i -t"
OPTS="-d -P"
OPTS="$OPTS --privileged=true "

##=== Invoke docker to instantiate image into container and run processor ====

#-- Define command to invoke inside container
DOCKER_COMMAND="/bin/bash -x ${IMG_PATH}/${procElem} ${IMG_PATH}/dummy.cfg"

#-- Make Docker call and get return status
dockerID=$($SUDO docker run ${OPTS} ${MAPPED_DIRS} ${DOCKER_IMAGE} ${DOCKER_COMMAND})
rc=$?

#-- Save container id. in local file
echo $dockerID > ${LOCAL_PATH}/docker.id

#-- Return to initial folder and return status code
cd ${SCRIPT_PATH}
exit $rc
