#!/bin/bash
##############################################################################
# File       : pgsql_initdb.sh - PostgreSQL Database initialisation
# Domain     : QPF.scripts
# Version    : 1.0
# Date       : 2016/09/01
# Copyright (C) 2015, 2016 J C Gonzalez
#_____________________________________________________________________________
# Purpose    : PostgreSQL Database initialisation
# Created by : J C Gonzalez
# Status     : Prototype
# History    : See <Changelog>
###############################################################################

rm -rf /home/eucops/opt/pgsql/data 
mkdir -p  /home/eucops/opt/pgsql/data
pg_ctl -D /home/eucops/opt/pgsql/data initdb

 
