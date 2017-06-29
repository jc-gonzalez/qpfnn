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
project (filehdl)

set (libfilehdl_hdr
  genfhdl.h
  jsonfhdl.h
  qdtrephdl.h
)

set (libfilehdl_src
  jsonfhdl.cpp
  qdtrephdl.cpp
)

#===== Project sections/libraries =======

add_library(filehdl SHARED ${libfilehdl_src})
target_include_directories (filehdl PUBLIC .
  ${PSQLINCDIR}
  ${NNMSG_ROOT_DIR}/include
  ${JSON_ROOT_DIR}
  ${INFIX_ROOT_DIR}
  ${NNCOMM_ROOT_DIR}
  ${TOOLS_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${FILEHDL_ROOT_DIR}
  ${PCRE2LIBDIR}
  ${PSQLLIBDIR}
  ${MONGOOSEDIR})
target_link_libraries (filehdl
  json infix str log tools uuidxx nncomm
  nanomsg mongoose
  ${PCRE2LIB} ${PSQLLIB}
  uuid pthread)
set_target_properties (filehdl PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS filehdl
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
