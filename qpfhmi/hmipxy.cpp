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
    : Component(name, addr, s)
{
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
HMIProxy::HMIProxy(std::string name, std::string addr, Synchronizer * s)
    : Component(name, addr, s)
{
}

//----------------------------------------------------------------------
// Method: runEachIteration
//----------------------------------------------------------------------
void HMIProxy::runEachIteration()
{
    if ((iteration % 10) == 0) {
        // Create message and send
        Message<MsgBodyCMD> msg;
        MsgBodyCMD body;
        msg.buildHdr(ChnlHMICmd, MsgHMICmd, CHNLS_IF_VERSION,
                     compName, "*",
                     "", "", "");
        body["cmd"] = CmdStates;
        msg.buildBody(body);

        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        std::string chnl(ChnlHMICmd);
        it = connections.find(chnl);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            conn->setMsgOut(msg.str());
            TRC("Sending request via channel " + chnl);
            TRC("with message: " + msg.str());
        }
    }
}

//----------------------------------------------------------------------
// Method: processHMICmdMsg
//----------------------------------------------------------------------
void HMIProxy::processHMICmdMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    Message<MsgBodyCMD> msg(m);
    std::string cmd = msg.body["cmd"].asString();
    TRC("MSG: " + m);
    if (cmd == CmdStates) {

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

//}
