/******************************************************************************
 * File:    hmipxy.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.HMIProxy
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
 *   Declare HMIProxy class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   CommNode
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
//   none
//------------------------------------------------------------

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
namespace QPF {

//==========================================================================
// Class: HMIProxy
//==========================================================================
class HMIProxy : public Component {

public:
    HMIProxy(const char * name = 0);

    //----------------------------------------------------------------------
    // Method: concurrentRun
    // Method executed when the thread is created (for QtConcurrent)
    //----------------------------------------------------------------------
    int concurrentRun();

    std::map<std::string, std::string> nodeStates;

    //----------------------------------------------------------------------
    // Method: sendNewCfgInfo
    //----------------------------------------------------------------------
    void sendNewCfgInfo();

    //----------------------------------------------------------------------
    // Method: sendMinLogLevel
    //----------------------------------------------------------------------
    void sendMinLogLevel(std::string lvlStr);

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

protected:
    //----------------------------------------------------------------------
    // Method: fromInitialisedToRunning
    //----------------------------------------------------------------------
    virtual void fromInitialisedToRunning();
    
    //----------------------------------------------------------------------
    // Method: execAdditonalLoopTasks
    //----------------------------------------------------------------------
    virtual void execAdditonalLoopTasks();

   //----------------------------------------------------------------------
    // Method: processTASK_RES
    //----------------------------------------------------------------------
    virtual void processMONIT_INFO();

};

}

#endif  /* HMIPXY_H */
