/******************************************************************************
 * File:    genfhdl.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.GenericFileHandler
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
 *   Declare GenericFileHandler class
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

#ifndef GENFHDL_H
#define GENFHDL_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   none
//------------------------------------------------------------
#include <string>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   none
//------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
// namespace QPF {

//==========================================================================
// Class: GenericFileHandler
//==========================================================================
class GenericFileHandler {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    GenericFileHandler(const char * fname = 0)
        : fileName(std::string(fname))
    {}

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    GenericFileHandler(std::string fname = std::string())
        : fileName(fname)
    {}

    //----------------------------------------------------------------------
    // (Virtual) Destructor
    //----------------------------------------------------------------------
    virtual ~GenericFileHandler() {}

public:
    //----------------------------------------------------------------------
    // Method: read
    // Reads the file and stores the retrieved data internally
    //----------------------------------------------------------------------
    virtual bool read() = 0;

    //----------------------------------------------------------------------
    // Method: write
    // Write the internal data in the file
    //----------------------------------------------------------------------
    virtual bool write() = 0;

protected:
    std::string fileName;
};

// }

#endif  /* GENFHDL_H */
