/******************************************************************************
 * File:    httpserver.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.HttpServer
 *
 * Version:  1.1
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declare Httpserver class
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

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - thread
//------------------------------------------------------------
#include <thread>
#include <mutex>
#include <unistd.h>
#include <stdlib.h>

#include <mongoose/Server.h>
#include <mongoose/WebController.h>

using namespace Mongoose;

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - component.h
//------------------------------------------------------------
#include "component.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: HttpServer
//==========================================================================
class HttpServer : public Component, public WebController {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    HttpServer(const char * name, const char * addr = 0, bool serveFiles = false);

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    HttpServer(std::string name, std::string addr = std::string(), bool serveFiles = false);

public:
    //----------------------------------------------------------------------
    // Method: hello
    //----------------------------------------------------------------------
    void hello(Request &request, StreamResponse &response);

    //----------------------------------------------------------------------
    // Method: form
    //----------------------------------------------------------------------
    void form(Request &request, StreamResponse &response);

    //----------------------------------------------------------------------
    // Method: formPost
    //----------------------------------------------------------------------
    void formPost(Request &request, StreamResponse &response);

    //----------------------------------------------------------------------
    // Method: session
    //----------------------------------------------------------------------
    void session(Request &request, StreamResponse &response);

    //----------------------------------------------------------------------
    // Method: forbid
    //----------------------------------------------------------------------
    void forbid(Request &request, StreamResponse &response);

    //----------------------------------------------------------------------
    // Method: exception
    //----------------------------------------------------------------------
    void exception(Request &request, StreamResponse &response);

    //----------------------------------------------------------------------
    // Method: uploadForm
    //----------------------------------------------------------------------
    void uploadForm(Request &request, StreamResponse &response);

    //----------------------------------------------------------------------
    // Method: uploadFormExecute
    //----------------------------------------------------------------------
    void uploadFormExecute(Request &request, StreamResponse &response);

    //----------------------------------------------------------------------
    // Method: setup
    //----------------------------------------------------------------------
    void setup();

    //----------------------------------------------------------------------
    // Method: run
    //----------------------------------------------------------------------
    virtual void run();

protected:
    //----------------------------------------------------------------------
    // Method: fromRunningToOperational
    //----------------------------------------------------------------------
    virtual void fromRunningToOperational();

private:
    //----------------------------------------------------------------------
    // Method: step
    //----------------------------------------------------------------------
    void timeStep();

private:
    bool enableSwarmRequests;

};

//}

#endif  /* HTTPSERVER_H */
