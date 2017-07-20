#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015,2016,2017 
# Copyright (C) 2015-2017  Euclid SOC Team at ESAC
#======================================================================
include (../common.cmake)

#===== tests =====
project (tests)

enable_testing()

include (CMakeLists.txt.gtest)
#set (GTESTDIR $ENV{GTESTDIR})
#include_directories(${GTESTDIR}/include)

set (unitTestsSet_hdr
    infix/test_infixeval.h
    )

set (unitTestsSet_src
    main.cpp
    infix/test_infixeval.cpp
    )

add_executable(unitTestsSet ${unitTestsSet_src})
add_dependencies (unitTestsSet json sdc infix qpf libcomm)
target_include_directories (unitTestsSet PUBLIC . ..
  ${NNMSG_ROOT_DIR}/include
  ${NNCOMM_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${INFIX_ROOT_DIR}
  ${PSQLLIBDIR})
target_link_libraries (unitTestsSet
  fmk json nncomm infix str log uuidxx
  nanomsg
  ${UUIDLIB} ${PSQLLIB}
  pthread
  ${googleTestLibraries})


add_test (NAME unitTestsSet COMMAND unitTests)
