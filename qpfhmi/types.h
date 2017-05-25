/******************************************************************************
 * File:    types.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.types
 *
 * Version:  1.2
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declaration of several datatypes for QPF HMI
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   none
 *
 * Files read / modified:
 *   none
 *
 * History:
 *   See <Changelog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/
#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>

#define TLIST_PROD_BASIC_MDATA_FIELDS      \
    T( productId ),                             \
    T( productType ),                           \
    T( productStatus ),                         \
    T( productVersion ),                        \
    T( productSize ),                           \
    T( creator ),                               \
    T( instrument ),                            \
    T( obsmode ),                               \
    T( startTime ),                             \
    T( endTime ),                               \
    T( regTime ),                               \
    T( url )                                    \
    
namespace QPF {

extern const char * MdataFieldNames[];

extern const int NumOfMdataFieldNames;

#define T(a) PRODMETA_FLD_ ## a
enum MdataFieldIdx { TLIST_PROD_BASIC_MDATA_FIELDS };
#undef T
 
struct MdataField {
    MdataField(std::string n, std::string v) :
        name(n), value(v) {}
    std::string name;
    std::string value;
};

typedef std::vector<MdataField> Mdata;

typedef std::vector<Mdata*>     MdataDB;

}

#endif // TYPES_H

