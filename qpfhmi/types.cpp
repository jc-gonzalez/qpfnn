/******************************************************************************
 * File:    types.cpp
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
 *   Provides object implementation for some declarations
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

#include "types.h"

namespace QPF {

#define T(a) #a
const char * MdataFieldNames[] = { TLIST_PROD_BASIC_MDATA_FIELDS };
#undef T

const int NumOfMdataFieldNames = sizeof(MdataFieldNames) / sizeof(char*);

}
