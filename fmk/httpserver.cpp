/******************************************************************************
 * File:    httpserver.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.httpserver
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
 *   Implement HttpServer class
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

#include "httpserver.h"

#include "log.h"
#include "tools.h"
#include "str.h"

#include "config.h"

using Configuration::cfg;

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
HttpServer::HttpServer(const char * name, const char * addr, bool serveFiles)
    : Component(name, addr, 0), enableSwarmRequests(serveFiles)
{
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
HttpServer::HttpServer(std::string name, std::string addr, bool serveFiles)
    : Component(name, addr, 0), enableSwarmRequests(serveFiles)
{
}

//----------------------------------------------------------------------
// Method: hello
//----------------------------------------------------------------------
void HttpServer::hello(Request &request, StreamResponse &response)
{
    response << "Hello "
             << htmlEntities(request.get("name", "... what's your name ?")) << std::endl;
}

//----------------------------------------------------------------------
// Method: form
//----------------------------------------------------------------------
void HttpServer::form(Request &request, StreamResponse &response)
{
    response << "<form method=\"post\">" << std::endl;
    response << "<input type=\"text\" name=\"test\" /><br >" << std::endl;
    response << "<input type=\"submit\" value=\"Envoyer\" />" << std::endl;
    response << "</form>" << std::endl;
}

//----------------------------------------------------------------------
// Method: formPost
//----------------------------------------------------------------------
void HttpServer::formPost(Request &request, StreamResponse &response)
{
    response << "Test=" << htmlEntities(request.get("test", "(unknown)"));
}

//----------------------------------------------------------------------
// Method: session
//----------------------------------------------------------------------
void HttpServer::session(Request &request, StreamResponse &response)
{
    Session & session = getSession(request, response);

    if (session.hasValue("try")) {
        response << "Session value: " << session.get("try");
    } else {
        ostringstream val;
        val << time(NULL);
        session.setValue("try", val.str());
        response << "Session value set to: " << session.get("try");
    }
}

//----------------------------------------------------------------------
// Method: forbid
//----------------------------------------------------------------------
void HttpServer::forbid(Request &request, StreamResponse &response)
{
    response.setCode(HTTP_FORBIDDEN);
    response << "403 forbidden demo";
}

//----------------------------------------------------------------------
// Method: exception
//----------------------------------------------------------------------
void HttpServer::exception(Request &request, StreamResponse &response)
{
    throw std::string("<b>HttpServer Exception!!</b>");
}


//----------------------------------------------------------------------
// Method: uploadForm
//----------------------------------------------------------------------
void HttpServer::uploadForm(Request &request, StreamResponse &response)
{
    response << "<h1>File upload demo (don't forget to create a tmp/ directory)</h1>";
    response << "<form enctype=\"multipart/form-data\" method=\"post\">";
    response << "Choose a file to upload: <input name=\"file\" type=\"file\" /><br />";
    response << "<input type=\"submit\" value=\"Upload File\" />";
    response << "</form>";
}


//----------------------------------------------------------------------
// Method: uploadFormExecute
//----------------------------------------------------------------------
void HttpServer::uploadFormExecute(Request &request, StreamResponse &response)
{
    request.handleUploads();

    // Iterate through all the uploaded files
    std::vector<UploadFile>::iterator it = request.uploadFiles.begin();
    for (; it != request.uploadFiles.end(); it++) {
        UploadFile file = *it;
        file.saveTo("tmp/");
        response << "Uploaded file: " << file.getName() << std::endl;
    }
}

//----------------------------------------------------------------------
// Method: setup
//----------------------------------------------------------------------
void HttpServer::setup()
{
    // Hello demo
    addRoute("GET",  "/hello",     HttpServer, hello);
    addRoute("GET",  "/",          HttpServer, hello);

    // Form demo
    addRoute("GET",  "/form",      HttpServer, form);
    addRoute("POST", "/form",      HttpServer, formPost);

    // Session demo
    addRoute("GET",  "/session",   HttpServer, session);

    // Exception example
    addRoute("GET",  "/exception", HttpServer, exception);

    // 403 demo
    addRoute("GET",  "/403",       HttpServer, forbid);

    // File upload demo
    addRoute("GET",  "/upload",    HttpServer, uploadForm);
    addRoute("POST", "/upload",    HttpServer, uploadFormExecute);
}

//----------------------------------------------------------------------
// Method: fromRunningToOperational
//----------------------------------------------------------------------
void HttpServer::fromRunningToOperational()
{
    int t;
    t = time(NULL);
    srand(t);

    Server server(8080);
    server.registerController(this);
    server.setOption("document_root", ".");
    server.setOption("enable_directory_listing", "false");
    server.start();

    InfoMsg("Server started, routes:");
    this->dumpRoutes();

    transitTo(OPERATIONAL);
    InfoMsg("New state: " + getStateName(getState()));
}

//----------------------------------------------------------------------
// Method: step
//----------------------------------------------------------------------
void HttpServer::timeStep()
{
    const long int stepSize = 200; // ms

    // Sleep until next second
    /*
      std::time_t tt = system_clock::to_time_t(system_clock::now());
      struct std::tm * ptm = std::localtime(&tt);
      ptm->tm_sec++;
      std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));
    */
    // Sleep for 'stepSize' milliseconds
    //auto start = std::chrono::high_resolution_clock::now();
    long int msnow =
        static_cast<long int>(std::chrono::duration_cast<
                              std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()
                                                         .time_since_epoch()).count());
    long int ms2wait = stepSize - ((msnow + stepSize) % stepSize);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms2wait));
}

//----------------------------------------------------------------------
// Method: run
//----------------------------------------------------------------------
void HttpServer::run()
{
    // State: Initialised
    // Transition to: Running
    fromInitialisedToRunning();

    // State: Initialised
    // Transition to: Running
    fromRunningToOperational();

    do {
        runEachIteration();
        timeStep();
    } while (getState() == OPERATIONAL);

    // State: Initialised
    // Transition to: Running
    fromOperationalToRunning();

    // State: Initialised
    // Transition to: Running
    fromRunningToOff();
}

//}
