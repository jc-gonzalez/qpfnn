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
    // Request task for processing in case the agent is idle
    if ((iteration % 100) == 0) {
        // Create message and send
        Message<MsgBodyCMD> msg;
        msg.buildHdr(ChnlHMICmd,
                     ChnlHMICmd,
                     "1.0",
                     compName,
                     "*",
                     "", "", "");

        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        std::string chnl(ChnlHMICmd);
        it = connections.find(chnl);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            conn->setMsgOut(msg.str());
            InfoMsg("Sending request via channel " + chnl);
        }
    }
}

//----------------------------------------------------------------------
// Method: processIncommingMessages
//----------------------------------------------------------------------
void HMIProxy::processIncommingMessages()
{
    MessageString m;
    for (auto & kv: connections) {
        const ChannelDescriptor & chnl = kv.first;
        ScalabilityProtocolRole * conn = kv.second;
        while (conn->next(m)) {
            MessageBase msg(m);
            std::string tgt(msg.header.target());
            if ((tgt != "*") && (tgt != compName)) { continue; }
            std::string type(msg.header.type());
            DBG(compName << " received the message [" << m << "] through the channel " + chnl);
            if      (chnl == ChnlHMICmd)    { processHMICmdMsg(conn, m); }
            else if (type == MsgFmkMon)     { processFmkMonMsg(conn, m); }
            else if (type == MsgTskRepDist) { processTskRepDistMsg(conn, m); }
            else    { WarnMsg("Message from unidentified channel " + chnl); }
        }
    }
}

//----------------------------------------------------------------------
// Method: processHMICmdMsg
//----------------------------------------------------------------------
void HMIProxy::processHMICmdMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    MessageString ack = "Received info from Core: " + m;
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


//}
