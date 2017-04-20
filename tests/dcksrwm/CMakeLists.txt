#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2017
# Copyright (C) 2015-2017 Euclid SOC Team at ESAC
#======================================================================
include (../../common.cmake)

#===== Projec dir. =======
project (dckswrm)

set (dckswrm_src
  main.cpp
)

add_executable(dckswrm ${dckswrm_src})
target_include_directories (dckswrm PUBLIC .
  ${NNMSG_ROOT_DIR}/include
  ${NNCOMM_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${INFIX_ROOT_DIR}
  ${PSQLLIBDIR}
  ${MONGOOSEDIR})
target_link_libraries (dckswrm
  fmk json nncomm infix str log uuidxx
  nanomsg mongoose
  ${UUIDLIB} ${PSQLLIB}
  pthread)
set_target_properties (dckswrm PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS dckswrm
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
