#!/bin/bash
##############################################################################
# File       : CreateDockerImgs.sh - Generates docker images from Dockerfiles
# Domain     : QPF.run.dockerfiles
# Version    : 1.2
# Date       : 2016/12/18
# Copyright (C) 2015,2016,2017  J C Gonzalez
#_____________________________________________________________________________
# Purpose    : Generates docker images from Dockerfiles
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

#- Run make to execute docker build and create the images
make -k -C "${SCRIPT_PATH}"


