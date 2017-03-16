#!/bin/bash
##############################################################################
# File       : InstallCOTS.sh - COTS Installation and database initialisation
# Domain     : QPF.scripts
# Version    : 1.0
# Date       : 2016/09/01
# Copyright (C) 2015, 2016 J C Gonzalez
#_____________________________________________________________________________
# Purpose    : COTS Installation and database initialisation
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

status=0

#- Messages
_ONHDR="\e[1;49;93m"
_ONMSG="\e[1;49;92m"
_ONRUN="\e[0;49;32m"
_ONERR="\e[1;49;91m"
_OFF="\e[0m"
STEP=0

#- Options
QT="no" 
PGSQL="no"
ZMQ="no" 
PCRE="no" 
CURL="no" 
UUID="no"
DOCKER="no"

#- Other
DATE=$(date +"%Y%m%d%H%M%S")
LOG_FILE=./cots_${DATE}.log
VERSION=1.0
LDLIBS=$(echo $LD_LIBRARY_PATH | tr ':' ' ')

###### Handy functions

greetings () {
    say "${_ONHDR}==============================================================================="
    say "${_ONHDR} Euclid QLA Processing Framework"
    say "${_ONHDR} Version ${VERSION}"
    say "${_ONHDR} COTS Installation and database initialisation"
    say "${_ONHDR} Execution time-stamp: ${DATE}"
    say "${_ONHDR}==============================================================================="
    say ""
}

usage () {
    local opts="[ -h ] [ -A ] [ -a ] [ -q ] [ -p ] [ -z ] [ -r ] [ -c ] [ -u ] [ -d ]"
    say "Usage: ${SCRIPT_NAME} $opts"
    say "where:"
    say "  -h         Show this usage message"
    say "  -A         Install all COTS, INCLUDING Qt framework"
    say "  -a         Install all COTS, EXCLUDING Qt framework"
    say "  -q         Install Qt framework"
    say "  -p         Install PostgreSQL, initialize database and restart server"
    say "  -z         Install ZeroMQ"
    say "  -r         Install PCRE2"
    say "  -c         Install libcurl"
    say "  -u         Install UUID library"
    say "  -d         Install Docker"
    say ""
    exit 1
}

say () {
    echo -e "$*${_OFF}"
    echo -e "$*" | sed -e 's#.\[[0-9];[0-9][0-9];[0-9][0-9]m##g' >> "${LOG_FILE}"
}

step () {
    say "${_ONMSG}## STEP ${STEP} - $*"
    STEP=$(($STEP + 1))
}

die () {
    say "${_ONERR}ERROR: $*"
    exit 1
}

###### Start

#### Parse command line and display grettings

## Parse command line
while getopts :hAaqpzrcud OPT; do
    case $OPT in
        h|+h) usage 
              ;;
        A|+A) QT="yes" 
              PGSQL="yes"
              ZMQ="yes" 
              PCRE="yes" 
              CURL="yes" 
              UUID="yes" 
              DOCKER="yes" 
              ;;
        a|+a) QT="no" 
              PGSQL="yes"
              ZMQ="yes" 
              PCRE="yes" 
              CURL="yes" 
              UUID="yes" 
              DOCKER="yes" 
              ;;
        q|+q) QT="yes" 
              ;;
        p|+p) PGSQL="yes" 
              ;;
        z|+z) ZMQ="yes" 
              ;;
        r|+r) PCRE="yes" 
              ;;
        c|+c) CURL="yes" 
              ;;
        u|+u) UUID="yes" 
              ;;
        d|+d) DOCKER="yes" 
              ;;
        *)    usage 
              exit 2
              ;;
    esac
done
shift `expr $OPTIND - 1`
OPTIND=1

## Say hello
greetings

## Checking required applications and libraries are installed

#### Installing COTS: I - Install PostgreSQL

if [ "${PGSQL}" == "yes" ]; then 
    step "Installing PostgreSQL"

    # We need to install PostgreSQL and then setup the database

    # For LODEEN (CentOS based), the currently available PostgreSQL
    # version by default is 9.2, but we need 9.4+
    # We install a PGDG for 9.6, and then install the 9.6 packages
    PSQL_PGDG="https://download.postgresql.org/pub/repos/yum/9.6/redhat/rhel-7-x86_64/pgdg-centos96-9.6-3.noarch.rpm" 
    PSQL_PKGS="postgresql96.x86_64" 
    PSQL_PKGS="$PSQL_PKGS postgresql96-devel.x86_64" 
    PSQL_PKGS="$PSQL_PKGS postgresql96-libs.x86_64" 
    PSQL_PKGS="$PSQL_PKGS postgresql96-server.x86_64" 
    PSQL_PKGS="$PSQL_PKGS postgresql96-docs.x86_64"
    
    #- 1. Installing PostgreSQL
    say ". Installing packages"
    sudo yum -y install ${PSQL_PGDG}
    sudo yum -y install ${PSQL_PKGS}

    #- 2. Setting up the database

    # Give access to internal folder:
    say ". Preparing internal folders"
    sudo chmod 777 /var/run/postgresql
    sudo chown postgres.postgres /var/run/postgresql
    sudo mkdir -p /opt
    sudo ln -s /usr/pgsql-9.6 /usr/pgsql
    sudo ln -s /usr/pgsql-9.6 /opt/pgsql
    
    export PATH=$PATH:/usr/pgsql/bin
    
    # Then, for the creation of the local folder, initialization and server start, 
    # use the scripts =scripts/pgsql_start_server.sh= and =scripts/pgsql_initdb.sh=

    say ". Initializing database"
    source ${SCRIPT_PATH}/pgsql_initdb.sh

    say ". Starting server"
    source ${SCRIPT_PATH}/pgsql_start_server.sh
fi

#### Installing COTS: II - Install Qt

if [ "${QT}" == "yes" ]; then 
    step "Installing Qt framework"

    # Create QT5 list of packages
    sudo yum -y list qt5-*x86_64 | grep -v -- -examples|grep qt5- | awk '{print $1;}' | tee /tmp/qt5pkgs.list

    # Install packages
    sudo yum -y install $(cat /tmp/qt5pkgs.list)
fi

#### Installing COTS: III - Install 0MQ

if [ "${ZMQ}" == "yes" ]; then 
    step "Installing ZeroMQ library"
    sudo yum -y install zeromq.x86_64 zeromq-devel.x86_64
fi

#### Installing COTS: IV - Install PCRE2

if [ "${PCRE}" == "yes" ]; then 
    step "Installing PCRE2 library"
    sudo yum -y install pcre2*
fi

#### Installing COTS: V - Install curl

if [ "${CURL}" == "yes" ]; then 
    step "Installing Curl library"
    sudo yum -y install curl.x86_64 libcurl-devel.x86_64
fi

#### Installing COTS: VI - Install UUID

if [ "${UUID}" == "yes" ]; then 
    step "Installing UUID library"
    sudo yum -y install uuid-devel.x86_64 libuuid-devel.x86_64
fi

#### Installing COTS: VII - Install Docker

if [ "${DOCKER}" == "yes" ]; then 
    step "Installing Docker"
    say ". Installing packages"
    sudo yum -y install docker
    say ". Creating docker group"
    sudo groupadd docker
    say ". Adding user to docker group"
    sudo usermod -aG docker $(whoami)
    say ". Starting Docker service"
    sudo service docker start
fi

#### Finishing
step "Final comments"

say "Compilation/Installation finished."
say ""
say "-------------------------------------------------------------------------------"
say "Please, do not forget to include folder /usr/lib64 in the LD_LIBRARY_PATH "
say "variable, and the /usr/lib64/qt5/bin folder in the PATH variable, with these"
say "commands:" 
say "  export PATH=/usr/lib64/qt5/bin:/usr/pgsql/bin:\$PATH"
say "  export LD_LIBRARY_PATH=/usr/lib64:/usr/pgsql/lib:\$LD_LIBRARY_PATH"
say "Done."

