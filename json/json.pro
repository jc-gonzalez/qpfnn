#======================================================================
# QPF - small toy QLA Processing Framework
#======================================================================
# Library for JsonCpp, from amalgamated source
# Author: J C Gonzalez - 2015, 2016
#======================================================================

include(../defaults.pri)

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   -= qt

TEMPLATE = lib

TARGET = json

INCLUDEPATH += .

HEADERS += \
        json/json.h \
        json/json-forwards.h
        
SOURCES += \
        jsoncpp.cpp
        
        

