/******************************************************************************
 * File:    metadatareader.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.MetadataReader
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
 *   Declare MetadataReader class, wrapper around libuuid functions
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   Component
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

#ifndef METADATAREADER_H
#define METADATAREADER_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//  none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: External packages
//  none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//  - str
//  - datatypes
//------------------------------------------------------------
#include "str.h"
#include "datatypes.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: MetadataInfo
// Container for metadata info
//==========================================================================
class MetadataInfo : public JValue {};

//==========================================================================
// Class: MetadataReader
// Base class to get metadata info from data products
//==========================================================================
class MetadataReader {
public:
    MetadataReader() {}
    MetadataReader(std::string & fName) : fileName(fName) {}

    virtual bool setFile(std::string & fName) { return false; }
    virtual bool getMetadataInfo(MetadataInfo & md) { return false; }

protected:
    std::string fileName;
};

#endif // METADATAREADER_H
