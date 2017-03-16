#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2017
# Copyright (C) 2015-2017 Euclid SOC Team at ESAC
#======================================================================
cmake_minimum_required(VERSION 2.8)
cmake_policy (SET CMP0015 NEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#===== COTS =====

set (NNMSG_ROOT_DIR /usr/local)

set (UUIDINCDIR /usr/include)
set (UUIDLIBDIR /usr/lib64)
set (UUIDLIB uuid)

if (EXISTS /usr/pgsql-9.6)
  set (PSQLDIR /usr/pgsql-9.6)
  set (PSQLLIBDIR ${PSQLDIR}/lib)
  set (PSQLINCDIR ${PSQLDIR}/include)
else()
  set (PSQLLIBDIR /usr/lib/x86_64-linux-gnu)
  set (PSQLINCDIR /usr/include/postgresql)
endif()
set (PSQLLIB pq)

#===== Project libraries =====

set (JSON_ROOT_DIR    ../json)
set (NNCOMM_ROOT_DIR  ../nncomm)
set (TOOLS_ROOT_DIR   ../tools)
set (LOG_ROOT_DIR     ../log)
set (STR_ROOT_DIR     ../str)
set (UUID_ROOT_DIR    ../uuid)
set (FMK_ROOT_DIR     ../fmk)

#==== Common directives

link_directories (
  ${NNMSG_ROOT_DIR}/lib
  ${NNCOMM_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${TOOLS_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${PSQLLIBDIR})
