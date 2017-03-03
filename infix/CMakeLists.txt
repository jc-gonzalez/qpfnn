#======================================================================
# InFix - Simple Infix Expressions Evaluation Library
#======================================================================
# Library project file
# Author: J C Gonzalez - 2015-2017
#======================================================================

project (infix)

set (infixLib_hdr
  infixeval.h)

add_library (infix SHARED ${infixLib_hdr})
set_target_properties (infix PROPERTIES LINKER_LANGUAGE CXX)

install (TARGETS infix
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
