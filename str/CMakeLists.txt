#======================================================================
# Str - Simple Strging library
#======================================================================
# Library project file
# Author: J C Gonzalez - 2015-2017
#======================================================================

project (str)

set (strLib_hdr
  str.h)

set (strLib_src
  str.cpp)

add_library (str SHARED ${strLib_src})
set_target_properties (str PROPERTIES LINKER_LANGUAGE CXX)

install (TARGETS str
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
