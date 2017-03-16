#!/bin/bash

FLDR=$1

if [ -z "$FLDR" ]; then
    FLDR=${HOME}/qpf
fi

## Remove old run folders
rm -rf ${FLDR}/run/20* ${FLDR}/log/qpfhmi_*

## Clear archive and inbox
rm -rf ${FLDR}/data/archive/in/* 
rm -rf ${FLDR}/data/archive/out/*
rm -rf ${FLDR}/data/inbox/*

## Clear database
cat <<EOF>/tmp/clean-up-qpfdb.sql
delete from products_info where id>0;
delete from transmissions where id>0;
delete from tasks_info where id>0;
EOF

psql -f /tmp/clean-up-qpfdb.sql qpfdb

rm -rf /tmp/clean-up-qpfdb.sql

