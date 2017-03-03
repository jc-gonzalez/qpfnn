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
  component.h
  datamng.h
  evtmng.h
  logmng.h
  tskage.h
  tskmng.h
  tskorc.h
  message.h
)

set (libfmk_src
  component.cpp
  datamng.cpp
  evtmng.cpp
  logmng.cpp
  tskage.cpp
  tskmng.cpp
  tskorc.cpp
)

#===== Project sections/libraries =======

set (NNMSG_ROOT_DIR /usr/local)
set (JSON_ROOT_DIR ../json)
set (NNCOMM_ROOT_DIR ../nncomm)
set (FMK_ROOT_DIR ../fmk)

link_directories ( ${NNMSG_ROOT_DIR}/lib ${NNCOMM_ROOT_DIR} ${FMK_ROOT_DIR} ${JSON_ROOT_DIR} )

add_library(fmk SHARED ${libfmk_src})
target_include_directories (fmk PUBLIC .
  ${NNMSG_ROOT_DIR}/include
  ${JSON_ROOT_DIR}
  ${NNCOMM_ROOT_DIR}
  ${FMK_ROOT_DIR})
target_link_libraries (fmk json nncomm nanomsg pthread)
set_target_properties (fmk PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS fmk
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
