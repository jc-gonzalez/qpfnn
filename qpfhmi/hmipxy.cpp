/******************************************************************************
 * File:    hmipxy.cpp
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

#include "hmipxy.h"

#include "log.h"
#include "tools.h"
#include "channels.h"
#include "message.h"
#include "str.h"
#include "hostinfo.h"

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
HMIProxy::HMIProxy(const char * name, const char * addr, Synchronizer * s)
    : Component(name, addr, s), requestQuit(false)
{
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
HMIProxy::HMIProxy(std::string name, std::string addr, Synchronizer * s)
    : Component(name, addr, s), requestQuit(false)
{
}

//----------------------------------------------------------------------
// Method: runEachIteration
//----------------------------------------------------------------------
void HMIProxy::runEachIteration()
{
    bool rqstSessionId = (iteration == 1); // Request session id
    bool rqstStates    = ((iteration % 10) == 0); // Request states

    if (rqstSessionId || rqstStates || requestQuit) {

        Message<MsgBodyCMD> msg;
        MsgBodyCMD body;
        msg.buildHdr(ChnlHMICmd, MsgHMICmd, CHNLS_IF_VERSION,
                     compName, "*",
                     "", "", "");

        if (rqstSessionId) {
            // Create message and send
            body["cmd"] = CmdSession;
        }

        if (rqstStates) { // Request states
            // Create message and send
            body["cmd"] = CmdStates;
        }

        if (requestQuit) {
            // Create message and send
            body["cmd"] = CmdQuit;
        }

        msg.buildBody(body);
        send(ChnlHMICmd, msg.str());
    }
}

//----------------------------------------------------------------------
// Method: processHMICmdMsg
//----------------------------------------------------------------------
void HMIProxy::processHMICmdMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    Message<MsgBodyCMD> msg(m);
    std::string cmd = msg.body["cmd"].asString();
    if (cmd == CmdStates) {
        cfg.nodeStates.clear();
        json & mp = msg.body["states"];
        Json::Value::iterator it = mp.begin();
        while (it != mp.end()) {
            std::string node(it.key().asString());
            std::string stat((*it).asString());
            TRC(compName + " received the information that " +
                node + " has state " + stat);
            cfg.nodeStates[node] = stat;
            ++it;
        }
        cfg.nodeStates["QPFHMI"] = getStateName(getState());
    } else if (cmd == CmdSession) {
        std::string sessId = msg.body["sessionId"].asString();
        if (sessId != cfg.sessionId) {
            InfoMsg("Trying to monitor folder for master session id " + sessId);
            doInParent(parent, "linkSessionLogs", sessId);
        }
    }
}

//----------------------------------------------------------------------
// Method: processTskRepDistMsg
//----------------------------------------------------------------------
void HMIProxy::processTskRepDistMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    Message<MsgBodyTSK> msg(m);
    MsgBodyTSK & body = msg.body;
    TaskInfo task(body["info"]);

    std::string taskName  = task.taskName();
    TaskStatus taskStatus = TaskStatus(task.taskStatus());

    TRC("EvtMng: Processing TaskReport: " << taskName
        << " has status " << TaskStatusName[taskStatus]);
}

//----------------------------------------------------------------------
// Method: processFmkMonMsg
//----------------------------------------------------------------------
void HMIProxy::processFmkMonMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    Message<MsgBodyTSK> msg(m);
    MsgBodyTSK & body = msg.body;
    JValue fmkInfoData(body["info"]);

    TRC(fmkInfoData.str());
    TRC(Config::procFmkInfo->toJsonStr());
    Config::procFmkInfo->fromStr(fmkInfoData.str());
    TRC("@@@@@@@@@@ RECEIVED UOPDATE OF FMK INFO @@@@@@@@@@");
}

//----------------------------------------------------------------------
// Method: log
// Shows a log message with a given criticality
//----------------------------------------------------------------------
void HMIProxy::log(std::string s, Log::LogLevel lvl)
{
    LogMsg(lvl, s);
}

//----------------------------------------------------------------------
// Method: quit
// Sends a message to EvtMng requesting the QUIT for all components
//----------------------------------------------------------------------
void HMIProxy::quit()
{
    requestQuit = true;
}


//----------------------------------------------------------------------
// Method: sendCmd
// Send a command message to a target (or a set of them)
//----------------------------------------------------------------------
void HMIProxy::sendCmd(std::string target, std::string what, std::string value)
{
//    sendCMD(target, what, value);
}

//----------------------------------------------------------------------
// Method: sendMinLogLevel
//----------------------------------------------------------------------
void HMIProxy::sendMinLogLevel(std::string lvlStr)
{
//    sendMONIT_RQST("*", "set_min_log_level", lvlStr);
}

//----------------------------------------------------------------------
// Method: sendNewCfgInfo
//----------------------------------------------------------------------
void HMIProxy::sendNewCfgInfo()
{
//    sendMONIT_RQST("*", "set_new_cfg", cfg.str());
}

//----------------------------------------------------------------------
// Method: getNodeStates
//----------------------------------------------------------------------
std::map<std::string, std::string> HMIProxy::getNodeStates()
{
    return cfg.nodeStates;
}

//----------------------------------------------------------------------
// Method: declareParentConnection
//----------------------------------------------------------------------
void HMIProxy::declareParentConnection(void (*fptr)(void*,std::string,std::string),
                                       void * context)
{
    doInParent = fptr;
    parent     = context;
}

//}
