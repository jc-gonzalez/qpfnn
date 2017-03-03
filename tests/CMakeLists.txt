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
project (fmktest)

set (fmktest_src
  main.cpp
)

#===== Project sections/libraries =======

set (NNMSG_ROOT_DIR /usr/local)
set (NNCOMM_ROOT_DIR ../nncomm)
set (FMK_ROOT_DIR ../fmk)
set (JSON_ROOT_DIR ../json)
set (INFIX_ROOT_DIR ../infix)

link_directories (
  ${NNMSG_ROOT_DIR}/lib
  ${NNCOMM_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${INFIX_ROOT_DIR}
)

add_executable(fmktest ${fmktest_src})
target_include_directories (fmktest PUBLIC .
  ${NNMSG_ROOT_DIR}/include
  ${NNCOMM_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${INFIX_ROOT_DIR})
target_link_libraries (fmktest nanomsg nncomm json fmk infix pthread)
set_target_properties (fmktest PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS fmktest
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
