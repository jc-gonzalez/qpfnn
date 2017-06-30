/******************************************************************************
 * File:    qdtrephdl.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.QDTReportHandler
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
 *   Declare QDTReportHandler class
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

#ifndef QDTREPHDL_H
#define QDTREPHDL_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   none
//------------------------------------------------------------
#include <vector>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   none
//------------------------------------------------------------
#include "jsonfhdl.h"
#include "alert.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
// namespace QPF {

//==========================================================================
// Class: QDTReportHandler
//==========================================================================
class QDTReportHandler : public JsonFileHandler{

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    QDTReportHandler(const char * fname = 0)
        : JsonFileHandler(fname)
    {}

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    QDTReportHandler(std::string fname = std::string())
        : JsonFileHandler(fname)
    {}

    //----------------------------------------------------------------------
    // (Virtual) Destructor
    //----------------------------------------------------------------------
    virtual ~QDTReportHandler() {}

public:
    //----------------------------------------------------------------------
    // Method: getIssues
    // Retrieves from the data the list of issues found
    //----------------------------------------------------------------------
    bool getIssues(std::vector<Alert*> & v);

    //----------------------------------------------------------------------
    // Method: checkDiagnostic
    // Checks the results of a given diagnostic
    //----------------------------------------------------------------------
    void checkDiagnostic(Json::Value::iterator it,
                         std::string & location,
                         std::vector<Alert*> & issues);

};

// }

#endif  /* QDTREPHDL_H */
