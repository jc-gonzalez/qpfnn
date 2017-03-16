#!/bin/bash -x
##############################################################################
# runTask.sh
# Run a processing task names {procElem} with an input cfg {cfgFile}
#-----------------------------------------------------------------------------
# Usage:
#   runTask.sh  {procElem} {cfgFile}
##############################################################################

QPF_SYS_ROOT="${HOME}/qpf"
QPF_RUN="${QPF_SYS_ROOT}/run"
DOCKER_TOOLS_PATH="${QPF_RUN}/bin"
QPF_IMG_RUN="/qpf/run"

RUN_DOCKER=${DOCKER_TOOLS_PATH}/runDocker.sh

CURDIR=$(dirname $(pwd)/$0)

thisdir=$(pwd)
cd ${DOCKER_TOOLS_PATH}

usage() {
    echo "Usage:  runTask.sh  {procElem} {cfgFile}"
}

if [ $# -lt 2 ]; then
    usage
    exit 1
fi

procElem=$1
cfgFile=$2

localPath=$(dirname $cfgFile)
imgPath=${QPF_IMG_RUN}/$(basename $localPath)
${RUN_DOCKER} ${procElem} ${localPath} ${imgPath}
rc=$?

cd ${thisdir}

if [[ $rc != 0 ]]; then
    exit $rc
fi
