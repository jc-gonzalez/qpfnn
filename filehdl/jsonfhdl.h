/******************************************************************************
 * File:    jsonfhdl.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.JsonFileHandler
 *
 * Version:  1.2
 *
 * Date:    2017/04/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015-2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declare JsonFileHandler class
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

#ifndef JSONFHDL_H
#define JSONFHDL_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   none
//------------------------------------------------------------
#include "genfhdl.h"
#include "json/json.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
// namespace QPF {

//==========================================================================
// Class: JsonFileHandler
//==========================================================================
class JsonFileHandler : public GenericFileHandler {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    JsonFileHandler(const char * fname = 0)
        : GenericFileHandler(fname)
    {}

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    JsonFileHandler(std::string fname = std::string())
        : GenericFileHandler(fname)
    {}

    //----------------------------------------------------------------------
    // (Virtual) Destructor
    //----------------------------------------------------------------------
    virtual ~JsonFileHandler() {}

public:
    //----------------------------------------------------------------------
    // Method: read
    // Reads the file and stores the retrieved data internally
    //----------------------------------------------------------------------
    virtual bool read();

    //----------------------------------------------------------------------
    // Method: write
    // Write the internal data in the file
    //----------------------------------------------------------------------
    virtual bool write();

protected:
    Json::Value data;
};

// }

#endif  /* JSONFHDL_H */
