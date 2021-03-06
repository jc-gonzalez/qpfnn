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
project (tools)

set (libtools_hdr
  process.h
  propdef.h
  alert.h
  dwatcher.h
  metadatareader.h
  sm.h
  timer.h
  tools.h
)

set (libtools_src
  alert.cpp
  dwatcher.cpp
  sm.cpp
  timer.cpp
  tools.cpp
)

add_library(tools SHARED ${libtools_src})
target_include_directories (tools PUBLIC .
  ${STR_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${TOOLS_ROOT_DIR})
target_link_libraries (tools str json pthread)
set_target_properties (tools PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS tools
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
