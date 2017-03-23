/******************************************************************************
 * File:    hmipxy.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.hmipxy
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
 *   Implement HMIProxy class
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

#define PRESERVE_LOG_SYSTEM
#include "hmipxy.h"

#include "log.h"
using LibComm::Log;
#include "tools.h"

#include <sys/time.h>

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
namespace QPF {

//#define DUMP_TASK_CONTENT_TO_LOG

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
HMIProxy::HMIProxy(const char * name) :
    Component(name)
{
    std::cerr << "Creating MIProxy with name '" << name << "'\n";
    canProcessMessage(MSG_MONIT_INFO_IDX);
}

//----------------------------------------------------------------------
// Method: concurrentRun
// Method executed when the thread is created (for QtConcurrent)
//----------------------------------------------------------------------
int HMIProxy::concurrentRun()
{
    return this->run();
}

//----------------------------------------------------------------------
// Method: log
// Shows a log message with a given criticality
//----------------------------------------------------------------------
void HMIProxy::log(std::string s, Log::LogLevel lvl)
{
    logMsg(s, lvl, true);
}

//----------------------------------------------------------------------
// Method: sendCmd
// Send a command message to a target (or a set of them)
//----------------------------------------------------------------------
void HMIProxy::sendCmd(std::string target, std::string what, std::string value)
{
    sendCMD(target, what, value);
}

//----------------------------------------------------------------------
// Method: sendMinLogLevel
//----------------------------------------------------------------------
void HMIProxy::sendMinLogLevel(std::string lvlStr)
{
    sendMONIT_RQST("*", "set_min_log_level", lvlStr);
}

//----------------------------------------------------------------------
// Method: sendNewCfgInfo
//----------------------------------------------------------------------
void HMIProxy::sendNewCfgInfo()
{
    
    sendMONIT_RQST("*", "set_new_cfg", cfg.str());
}

//----------------------------------------------------------------------
// Method: fromInitialisedToRunning
//----------------------------------------------------------------------
void HMIProxy::fromInitialisedToRunning()
{
    // Establish communications with other peer
    establishCommunications();
    log("COMMUNICATION ESTABLISHED!!", Log::INFO);
}

//----------------------------------------------------------------------
// Method: execAdditonalLoopTasks
//----------------------------------------------------------------------
void HMIProxy::execAdditonalLoopTasks()
{
    sendMONIT_RQST("*", "state", "?");
}

//----------------------------------------------------------------------
// Method: processTASK_RES
//----------------------------------------------------------------------
void HMIProxy::processMONIT_INFO()
{
    Message_MONIT_RQST * msg = dynamic_cast<Message_MONIT_RQST *>(msgData.msg);
    
    std::map<std::string, std::string>::iterator it = msg->variables.paramList.find("state");
    if (it != msg->variables.paramList.end()) {
        nodeStates[msg->header.source] = (*it).second;
    }
}

}
