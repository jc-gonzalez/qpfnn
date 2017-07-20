/******************************************************************************
 * File:    hmipxy.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.HMIProxy
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
 *   Declare HMIProxy class
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

#ifndef HMIPXY_H
#define HMIPXY_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - thread
//------------------------------------------------------------
#include <thread>
#include <mutex>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - component.h
//   - procinfo.h
//------------------------------------------------------------
#include "component.h"
#include "procinfo.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: HMIProxy
//==========================================================================
class HMIProxy : public Component {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    HMIProxy(const char * name, const char * addr = 0, Synchronizer * s = 0);

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    HMIProxy(std::string name, std::string addr = std::string(), Synchronizer * s = 0);

    //----------------------------------------------------------------------
    // Method: log
    // Shows a log message with a given criticality
    //----------------------------------------------------------------------
    void log(std::string s, Log::LogLevel lvl = Log::INFO);

    //----------------------------------------------------------------------
    // Method: sendCmd
    // Send a command message to a target (or a set of them)
    //----------------------------------------------------------------------
    void sendCmd(std::string target, std::string what, std::string value);

    //----------------------------------------------------------------------
    // Method: sendMinLogLevel
    //----------------------------------------------------------------------
    void sendMinLogLevel(std::string lvlStr);

    //----------------------------------------------------------------------
    // Method: sendNewCfgInfo
    //----------------------------------------------------------------------
    void sendNewCfgInfo();

    //----------------------------------------------------------------------
    // Method: getNodeStates
    //----------------------------------------------------------------------
    std::map<std::string, std::string> getNodeStates();

    //----------------------------------------------------------------------
    // Method: quit
    // Sends a message to EvtMng requesting the QUIT for all components
    //----------------------------------------------------------------------
    void quit();

    //----------------------------------------------------------------------
    // Method: declareParentConnection
    //----------------------------------------------------------------------
    void declareParentConnection(void (*fptr)(void*,std::string,std::string),
                                 void * context);
    
protected:
    //----------------------------------------------------------------------
    // Method: runEachIteration
    //----------------------------------------------------------------------
    virtual void runEachIteration();

protected:
    //----------------------------------------------------------------------
    // Method: processHMICmdMsg
    //----------------------------------------------------------------------
    virtual void processHMICmdMsg(ScalabilityProtocolRole* c, MessageString & m);

    //----------------------------------------------------------------------
    // Method: processTskRepDistMsg
    //----------------------------------------------------------------------
    virtual void processTskRepDistMsg(ScalabilityProtocolRole* c, MessageString & m);

    //----------------------------------------------------------------------
    // Method: processFmkMonMsg
    //----------------------------------------------------------------------
    virtual void processFmkMonMsg(ScalabilityProtocolRole* c, MessageString & m);

private:
    bool requestQuit;

    void (*doInParent)(void *,std::string,std::string);
    void * parent;
};

//}

#endif  /* TASKAGENT_H */
