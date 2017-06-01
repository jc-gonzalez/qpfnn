/******************************************************************************
 * File:    urlhdl.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.URLHandler
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
 *   Declare URLHandler class
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

#ifndef URLHANDLER_H
#define URLHANDLER_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: External packages
//  - propref.h
//------------------------------------------------------------
#include "propdef.h"

//------------------------------------------------------------
// Topic: Project headers
//   - datatypes.h
//------------------------------------------------------------
#include "datatypes.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: URLHandler
//==========================================================================
class URLHandler {

public:
    URLHandler(bool remote = false);

    PropertyRef(URLHandler, ProductMetadata, product, Product);

    enum LocalArchiveMethod {
        LINK,
        MOVE,
        COPY,
        COPY_TO_REMOTE,
        COPY_TO_MASTER,
        SYMLINK
    };

public:

    //----------------------------------------------------------------------
    // Method: fromExternal2Inbox
    //----------------------------------------------------------------------
    ProductMetadata & fromExternal2Inbox();

    //----------------------------------------------------------------------
    // Method: fromOutbox2External
    //----------------------------------------------------------------------
    ProductMetadata & fromOutbox2External();

    //----------------------------------------------------------------------
    // Method: fromFolder2Inbox
    //----------------------------------------------------------------------
    ProductMetadata & fromFolder2Inbox();

    //----------------------------------------------------------------------
    // Method: fromInbox2Local
    //----------------------------------------------------------------------
    ProductMetadata & fromInbox2LocalArch(bool tx = true);

    //----------------------------------------------------------------------
    // Method: fromLocal2Gateway
    //----------------------------------------------------------------------
    ProductMetadata & fromLocalArch2Gateway();

     //----------------------------------------------------------------------
    // Method: fromGateway2Processing
    //----------------------------------------------------------------------
    ProductMetadata & fromGateway2Processing();

    //----------------------------------------------------------------------
    // Method: fromProcessing2Gateway
    //----------------------------------------------------------------------
    ProductMetadata & fromProcessing2Gateway();

     //----------------------------------------------------------------------
    // Method: fromGateway2Local
    //----------------------------------------------------------------------
    ProductMetadata & fromGateway2LocalArch();

    //----------------------------------------------------------------------
    // Method: relocate
    //----------------------------------------------------------------------
    int relocate(std::string & sFrom, std::string & sTo,
                 LocalArchiveMethod method = LINK, int msTimeOut = 0);

    //----------------------------------------------------------------------
    // Method: copyfile
    //----------------------------------------------------------------------
    int copyfile(std::string & sFrom, std::string & sTo);

    //----------------------------------------------------------------------
    // Method: rcopyfile
    //----------------------------------------------------------------------
    int rcopyfile(std::string & sFrom, std::string & sTo, bool toRemote);

    //----------------------------------------------------------------------
    // Method: runlink
    //----------------------------------------------------------------------
    int runlink(std::string & f);

    //----------------------------------------------------------------------
    // Method: setRemoteCopyParams
    //----------------------------------------------------------------------
    void setRemoteCopyParams(std::string maddr, std::string raddr);

    //----------------------------------------------------------------------
    // Method: setProcElemRunDir
    //----------------------------------------------------------------------
    void setProcElemRunDir(std::string wkDir, std::string tskDir);

private:
    std::string workDir;
    std::string intTaskDir;
    std::string taskExchgDir;
    std::string master_address;
    std::string remote_address;

    std::string productUrl;
    std::string productUrlSpace;

    bool isRemote;
};

//}

#endif  /* URLHANDLER_H */
