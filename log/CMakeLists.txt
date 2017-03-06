#======================================================================
# Log - Simple Logging library
#======================================================================
# Library project file
# Author: J C Gonzalez - 2015-2017
#======================================================================

project (log)

set (logLib_hdr
  log.h)

set (logLib_src
  log.cpp)

add_library (log SHARED ${logLib_src})
set_target_properties (log PROPERTIES LINKER_LANGUAGE CXX)

install (TARGETS log
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
