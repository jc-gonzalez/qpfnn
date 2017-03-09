#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2017
# Copyright (C) 2015-2017 Euclid SOC Team at ESAC
#======================================================================
cmake_minimum_required(VERSION 3.5)

#===== Projec dir. =======
project (fmk)

set (libfmk_hdr
  channels.h
  datatypes.h
  message.h
  config.h
  filenamespec.h
  component.h
#  dbhdl.h
#  dbhdlpostgre.h
  datamng.h
  evtmng.h
  logmng.h
  tskage.h
  tskmng.h
  tskorc.h
  sync.h
)

set (libfmk_src
  config.cpp
  filenamespec.cpp
  component.cpp
#  dbhdlpostgre.cpp
  datamng.cpp
  evtmng.cpp
  logmng.cpp
  tskage.cpp
  tskmng.cpp
  tskorc.cpp
)

#===== Project sections/libraries =======

set (NNMSG_ROOT_DIR /usr/local)

set (JSON_ROOT_DIR    ../json)
set (NNCOMM_ROOT_DIR  ../nncomm)
set (TOOLS_ROOT_DIR   ../tools)
set (LOG_ROOT_DIR     ../log)
set (STR_ROOT_DIR     ../str)
set (UUID_ROOT_DIR    ../uuid)
set (FMK_ROOT_DIR     ../fmk)

if (EXISTS /usr/pgsql-9.6)
  set (PSQLDIR /usr/pgsql-9.6)
endif()

set (PSQLLIBDIR /usr/lib/x86_64-linux-gnu)
set (PSQLINCDIR /usr/include/postgresql)
set (PSQLLIB pq)

link_directories (
  ${NNMSG_ROOT_DIR}/lib
  ${NNCOMM_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${TOOLS_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${PSQLLIBDIR}
)

add_library(fmk SHARED ${libfmk_src})
target_include_directories (fmk PUBLIC .
  ${PSQLINCDIR}
  ${NNMSG_ROOT_DIR}/include
  ${JSON_ROOT_DIR}
  ${NNCOMM_ROOT_DIR}
  ${TOOLS_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${FMK_ROOT_DIR})
target_link_libraries (fmk
  json str log tools uuid nncomm
  nanomsg ${PSQLLIB} pthread)
set_target_properties (fmk PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS fmk
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
