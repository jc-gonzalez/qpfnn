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
  propdef.h
  dwatcher.h
  sm.h
  tools.h
)

set (libtools_src
  dwatcher.cpp
  sm.cpp
  tools.cpp
)

add_library(tools SHARED ${libtools_src})
target_include_directories (tools PUBLIC .
  ${JSON_ROOT_DIR}
  ${TOOLS_ROOT_DIR})
target_link_libraries (tools json pthread)
set_target_properties (tools PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS tools
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)