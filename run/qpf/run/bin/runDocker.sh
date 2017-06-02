#!/bin/bash -x
##############################################################################
# runDocker.sh
# Run a docker image, getting input parameters and files,
# storing them in a dropbox path, and recovering output data from the
# very same dropbox path
#-----------------------------------------------------------------------------
# Usage:
#   runDocker.sh  {procElem} {localPath} {imgPath}
##############################################################################

QPF_SYS_ROOT="${HOME}/qpf"
QPF_RUN="${QPF_SYS_ROOT}/run"
DOCKER_TOOLS_PATH="${QPF_RUN}/bin"
PROC_BIN_ROOT="${QPF_SYS_ROOT}/bin"

DOCKER_IMAGE="qpf-debian:a"

SUDO=""

usage() {
    echo "Usage:  runDocker.sh  {procElem} {localPath} {imgPath}"
}

if [ $# -lt 3 ]; then
    usage
    exit 1
fi
procElem=$1
localPath=$2
imgPath=$3

rm -rf ${localPath}/out ${localPath}/log
mkdir -p ${localPath}/out ${localPath}/log

ln ${DOCKER_TOOLS_PATH}/${procElem} ${localPath}/

prefix=${procElem%_*}
binPath=/qlabin

case ${procElem} in
    LE1_Processor|LE1_VIS_MetadataCollector|LE1_VIS_Ingestor)
        procBinPath=${PROC_BIN_ROOT}/QPFTestProcessors
        echo "${UID}" > ${localPath}/dummy.cfg
        ;;
    QLA_Processor)
        procBinPath=${PROC_BIN_ROOT}/QDT
        echo "${UID}" > ${localPath}/dummy.cfg
        ;;
    LE1_VIS_Processor|LE1_NISP_Processor)
        procBinPath=${PROC_BIN_ROOT}/QDT
        binPath=""
        DOCKER_IMAGE="le1-sim-debian:a"
        ls -1 ${localPath}/in/* | sed -e "s#${localPath}/##g" > ${localPath}/dummy.cfg
        ;;
    *)
        procBinPath=${PROC_BIN_ROOT}
esac

#OPTS="-i -t"
OPTS="-d -P"
OPTS="$OPTS --privileged=true "

declare -A map
map[1]="${localPath}:${imgPath}"
map[2]="${localPath}/in:${imgPath}/in"
map[3]="${localPath}/out:${imgPath}/out"
if [ -n "${binPath}" ]; then
    map[4]="${procBinPath}:${binPath}"
fi
MAPPED_DIRS=$(echo " ${map[@]}" |sed 's/ / -v /g')

DOCKER_COMMAND="/bin/bash ${imgPath}/${procElem} ${imgPath}/dummy.cfg"

dockerID=$($SUDO docker run ${OPTS} ${MAPPED_DIRS} ${DOCKER_IMAGE} ${DOCKER_COMMAND})

echo $dockerID > ${localPath}/docker.id
