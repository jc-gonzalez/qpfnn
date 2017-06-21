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
#set (NNMSG_ROOT_DIR /Users/jcgonzalez/ws/nanomsg/cots/nanomsg-1.0.0/src)

set (UUIDINCDIR /usr/include)
set (UUIDLIBDIR /usr/lib64)
set (UUIDLIB uuidxx)

set (PCRE2INCDIR /usr/include)
set (PCRE2LIBDIR /usr/lib64)
set (PCRE2LIB pcre2-posix)

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

#set (PWD /Users/jcgonzalez/ws/QPFnn)
set (PWD ..)

set (JSON_ROOT_DIR    ${PWD}/json)
set (NNCOMM_ROOT_DIR  ${PWD}/nncomm)
set (INFIX_ROOT_DIR   ${PWD}/infix)
set (TOOLS_ROOT_DIR   ${PWD}/tools)
set (LOG_ROOT_DIR     ${PWD}/log)
set (STR_ROOT_DIR     ${PWD}/str)
set (UUID_ROOT_DIR    ${PWD}/uuid)
set (FMK_ROOT_DIR     ${PWD}/fmk)

set (MONGOOSEDIR      ${PWD}/mongoose)

#==== Common directives

link_directories (
  ${NNMSG_ROOT_DIR}/lib
  ${NNCOMM_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${TOOLS_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${INFIX_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${PCRE2LIBDIR}
  ${PSQLLIBDIR}
)
