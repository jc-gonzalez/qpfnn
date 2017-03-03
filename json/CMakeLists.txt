#======================================================================
# JSON - JSON Parsing Library
#======================================================================
# Library project file
# Author: J C Gonzalez - 2015, 2016
#======================================================================

project (json)

set (jsonLib_src
  jsoncpp.cpp)

set (jsonLib_hdr
  json/json.h
  json/json-forwards.h)

add_library (json SHARED ${jsonLib_src})
install (TARGETS json
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
