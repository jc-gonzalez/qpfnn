#======================================================================
# Uuid - Simple Uuidging library
#======================================================================
# Library project file
# Author: J C Gonzalez - 2015-2017
#======================================================================

project (uuid)

set (uuidLib_hdr
  uuid.h)

set (uuidLib_src
  uuid.cpp)

add_library (uuid SHARED ${uuidLib_src})
set_target_properties (uuid PROPERTIES LINKER_LANGUAGE CXX)

install (TARGETS uuid
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
