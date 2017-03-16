#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2017
# Copyright (C) 2015-2017 Euclid SOC Team at ESAC
#======================================================================
cmake_minimum_required(VERSION 2.8)
cmake_policy (SET CMP0015 OLD)

include (common.cmake)

project (QPF)

set (PROJECT_NAME QPF)

set (QPF_MAJOR 1)
set (QPF_MINOR 0)
set (QPF_PATCH 1)

set (QPF_VERSION ${QPF_MAJOR}.${QPF_MINOR}.${QPF_PATCH})

message (STATUS "Building QPF version ${QPF_VERSION}")

set (BUILD_ID $ENV{BUILD_ID})
add_definitions(-DBUILD_ID="${BUILD_ID}")

add_subdirectory (json)
add_subdirectory (nncomm)
add_subdirectory (infix)
add_subdirectory (log)
add_subdirectory (str)
add_subdirectory (tools)
add_subdirectory (uuid)
add_subdirectory (fmk)
#add_subdirectory (qpf)
#add_subdirectory (qpfhmi)

if (HMI)
  message ("HMI component will be generated as well")
  add_subdirectory (qpfgui)
endif()

add_subdirectory (tests)

#== CPack configuration
set (CPACK_PACKAGE_VERSION_MAJOR ${QPF_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${QPF_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${QPF_PATCH})
set (CPACK_GENERATOR "TGZ")
set (CPACK_PACKAGING_INSTALL_PREFIX "")
include (CPack)
