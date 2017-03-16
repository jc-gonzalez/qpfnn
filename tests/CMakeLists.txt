#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2017
# Copyright (C) 2015-2017 Euclid SOC Team at ESAC
#======================================================================
include (../common.cmake)

#===== Projec dir. =======
project (fmktest)

set (fmktest_src
  main.cpp
)

add_executable(fmktest ${fmktest_src})
target_include_directories (fmktest PUBLIC .
  ${NNMSG_ROOT_DIR}/include
  ${NNCOMM_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${INFIX_ROOT_DIR}
  ${PSQLLIBDIR})
target_link_libraries (fmktest
  fmk json nncomm infix str log uuidxx
  nanomsg
  ${UUIDLIB} ${PSQLLIB}
  pthread)
set_target_properties (fmktest PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS fmktest
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
