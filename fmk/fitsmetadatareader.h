/******************************************************************************
 * File:    fitsmetadatareader.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.FitsMetadataReader
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
 *   Declare FitsMetadataReader class
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

#ifndef FITSMETADATAREADER_H
#define FITSMETADATAREADER_H

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
//  - metadatareader
//------------------------------------------------------------
#include "metadatareader.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: FitsMetadataReader
// Class to get metadata info from FITS data products
//==========================================================================
class FitsMetadataReader : public MetadataReader {

public:
    FitsMetadataReader();
    FitsMetadataReader(std::string & fName);
    ~FitsMetadataReader();

    virtual bool setFile(std::string & fName);
    virtual bool getMetadataInfo(MetadataInfo & md);
protected:
    bool readFitsMetadata(std::string & str);
    void clearComment(std::string & line);
    void clearQuotes(std::string & line);
};

#endif // FITSMETADATAREADER_H
