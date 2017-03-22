#======================================================================
# Uuid - Simple Uuid wrapper library
#======================================================================
# Library project file
# Author: J C Gonzalez - 2015-2017
#======================================================================
include (../common.cmake)

project (uuidxx)

set (uuidxxLib_hdr
  uuidxx.h)

set (uuidxxLib_src
  uuidxx.cpp)

link_directories (${UUIDLIBDIR})
include_directories (${UUIDINCDIR})

add_library (uuidxx SHARED ${uuidxxLib_src})
target_include_directories (uuidxx PUBLIC .)
target_link_libraries (uuidxx ${UUIDLIB})
set_target_properties (uuidxx PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS uuidxx
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
