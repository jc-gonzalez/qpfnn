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
project (fmk)

set (libfmk_hdr
  channels.h
  datatypes.h
  message.h
  config.h
  infixeval.h
  filenamespec.h
  component.h
  dbhdl.h
  dbhdlpostgre.h
  except.h
  datamng.h
  evtmng.h
  logmng.h
  tskage.h
  tskmng.h
  tskorc.h
  srvmng.h
  cntrmng.h
  httpserver.h
  metadatareader.h
  fitsmetadatareader.h
  hostinfo.h
  sync.h
  urlhdl.h
)

set (libfmk_src
  config.cpp
  datatypes.cpp
  filenamespec.cpp
  component.cpp
  dbhdlpostgre.cpp
  datamng.cpp
  evtmng.cpp
  logmng.cpp
  tskage.cpp
  tskmng.cpp
  tskorc.cpp
  srvmng.cpp
  cntrmng.cpp
  httpserver.cpp
  fitsmetadatareader.cpp
  hostinfo.cpp
  urlhdl.cpp
)

#===== Project sections/libraries =======

add_library(fmk SHARED ${libfmk_src})
target_include_directories (fmk PUBLIC .
  ${PSQLINCDIR}
  ${NNMSG_ROOT_DIR}/include
  ${JSON_ROOT_DIR}
  ${INFIX_ROOT_DIR}
  ${NNCOMM_ROOT_DIR}
  ${TOOLS_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${PCRE2LIBDIR}
  ${PSQLLIBDIR}
  ${MONGOOSEDIR})
target_link_libraries (fmk
  json infix str log tools uuidxx nncomm
  nanomsg mongoose
  ${PCRE2LIB} ${PSQLLIB}
  uuid pthread)
set_target_properties (fmk PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS fmk
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
