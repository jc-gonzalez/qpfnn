#!/bin/bash
##############################################################################
# File       : pgsql_start_server.sh - PostgreSQL Server start
# Domain     : QPF.scripts
# Version    : 1.0
# Date       : 2016/09/01
# Copyright (C) 2015, 2016 J C Gonzalez
#_____________________________________________________________________________
# Purpose    : PostgreSQL Server start
# Created by : J C Gonzalez
# Status     : Prototype
# History    : See <Changelog>
###############################################################################

# Prepare internal folder
sudo chmod 777 /var/run/postgresql
sudo chown -R postgres:postgres /var/run/postgresql

# Try to start server
pg_ctl -D /home/eucops/opt/pgsql/data -l /home/eucops/opt/pgsql/data/log start

# Check server status
status=$(pg_ctl -D /home/eucops/opt/pgsql/data status 2>&1 | head -1 | cut -d" " -f 4)
if [ "$status" == "running" ]; then
    echo "Server is now running"
else
    echo "ERROR: Server could not be started."
    exit 1
fi

