##############################################################################
# File       : GenerateRelease.sh - QPF Release Generation script
# Domain     : QPF.scripts
# Version    : 1.0
# Date       : 2016/04/01
# Copyright (C) 2015, 2016 J C Gonzalez
#_____________________________________________________________________________
# Purpose    : Generate QPF Release Package
# Created by : J C Gonzalez
# Status     : Prototype
# History    : See <Changelog>
###############################################################################

###### Script variables

#- This script path and name
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

#- QPF source code paths
QPF_PATH=$(dirname "${SCRIPT_PATH}")
QPF_PATH_SUPER=$(dirname "${QPF_PATH}")
QPF_DIRNAME=$(basename "${QPF_PATH}")

#- Messages
_ONHDR="\e[1;49;93m"
_ONMSG="\e[1;49;92m"
_ONRUN="\e[0;49;32m"
_ONERR="\e[1;49;91m"
_OFF="\e[0m"
STEP=0

#- Options
TARGET_FOLDER="."
VERSION=$(cat "${QPF_PATH}/VERSION")
TIMESTAMP=$(date +"%Y%m%d%H%M%S")

###### Handy functions

greetings () {
    say "${_ONHDR}==============================================================================="
    say "${_ONHDR} Euclid QLA Processing Framework"
    say "${_ONHDR} Version ${VERSION}"
    say "${_ONHDR} Release Package Generation"
    say "${_ONHDR} Build time-stamp: ${TIMESTAMP}"
    say "${_ONHDR}==============================================================================="
    say ""
}

usage () {
    local opts="[ -h ] [ -d <path> ] [ -v <ver> ] [ -t <time> ]"
    say "Usage: ${SCRIPT_NAME} $opts"
    say "where:"
    say "  -h         Show this usage message"
    say "  -d <path>  Folder to locate the generated package (default:${TARGET_FOLDER})"
    say "  -v <ver>   Version string (default:${VERSION})"
    say "  -t <time>  Set time stamp (default:${TIMESTAMP})"
    say ""
    exit 1
}

say () {
    echo -e "${_ONMSG}$*${_OFF}"
}

die () {
    say "${_ONERR}ERROR: $*"
    exit 1
}

###### Start

## Parse command line
while getopts :hd:v:t: OPT; do
    case $OPT in
        h|+h) usage ;;
        d|+d) TARGET_FOLDER="$OPTARG" ;;
        v|+v) VERSION="$OPTARG" ;;
        t|+t) TIMESTAMP="$OPTARG" ;;
        *)    usage ; exit 2
    esac
done
shift `expr $OPTIND - 1`
OPTIND=1

## Define final values
QPF_PKG="QPF-${VERSION}-${TIMESTAMP}.tar.gz"

n=0
((n++)); content[$n]="QPF.pro"
((n++)); content[$n]="env.sh"
((n++)); content[$n]="defaults.pri"
((n++)); content[$n]="README.md"
((n++)); content[$n]="INSTALL.md"
((n++)); content[$n]="VERSION"
((n++)); content[$n]="infix"
((n++)); content[$n]="json"
((n++)); content[$n]="libcomm"
((n++)); content[$n]="sdc"
((n++)); content[$n]="src"
((n++)); content[$n]="qpfhmi"
((n++)); content[$n]="scripts/BuildQPF.sh"
((n++)); content[$n]="scripts/RunQPFHMI.sh"
((n++)); content[$n]="scripts/GenerateRelease.sh"
((n++)); content[$n]="run/QPF-workarea.tgz"
((n++)); content[$n]="run/qpfdb.sql"
((n++)); content[$n]="tests"

QPF_DIST_CONTENT=$(echo " ${content[@]}" | sed -e "s# # ${QPF_DIRNAME}/#g")

## Say hello
greetings

## Ensuring target folder exists
say "Ensuring target folder ${TARGET_FOLDER} exists"

if [ ! -d "${TARGET_FOLDER}" ]; then
    say "  - Creating target folder ${TARGET_FOLDER}"
    mkdir -p "'${TARGET_FOLDER}'" || die "Cannot create folder ${TARGET_FOLDER}"
fi

## Creating package
say "Creating release package ${QPF_PKG} in folder ${TARGET_FOLDER}"

cd ${QPF_PATH_SUPER}
tar czf "${TARGET_FOLDER}/${QPF_PKG}" ${QPF_DIST_CONTENT}

## Finishing
say "QPF Release ${VERSION} Package generated"
ls -l "${TARGET_FOLDER}/${QPF_PKG}"

say ""

exit 0
