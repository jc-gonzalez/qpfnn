/******************************************************************************
 * File:    evtmng.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.evtmng
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
 *   Implement EvtMng class
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

#include "evtmng.h"

#include "filenamespec.h"
#include "message.h"
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
EvtMng::EvtMng(const char * name, const char * addr, Synchronizer * s)
    : Component(name, addr, s)
{
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
EvtMng::EvtMng(std::string name, std::string addr, Synchronizer * s)
    : Component(name, addr, s)
{
}

//----------------------------------------------------------------------
// Method: fromRunningToOperational
//----------------------------------------------------------------------
void EvtMng::fromRunningToOperational()
{
    // Install DirWatcher at inbox folder
    dw = new DirWatcher(Config::PATHBase + "/data/inbox");
    InfoMsg("DirW2tcher installed at " + Config::PATHBase + "/data/inbox");

    transitTo(OPERATIONAL);
    InfoMsg("New state: " + getStateName(getState()));
}

//----------------------------------------------------------------------
// Method: fromOperationalToRunning
//----------------------------------------------------------------------
void EvtMng::fromOperationalToRunning()
{
    InfoMsg("New state: " + getStateName(getState()));

    // stop watching inbox folder
    dw->stop();

    transitTo(OFF);
}

//----------------------------------------------------------------------
// Method: runEachIteration
//----------------------------------------------------------------------
void EvtMng::runEachIteration()
{
    // 1. Check DirWatcher events from inbox folder
    DirWatcher::DirWatchEvent e;
    while (dw->nextEvent(e)) {
        TraceMsg("New DirWatchEvent: " + e.path + "/" + e.name
                 + (e.isDir ? " DIR " : " ") + std::to_string(e.mask));

        // Process only files
        // TODO: Process directories that appear at inbox
        if (! e.isDir) {
            std::string file(e.path + "/" + e.name);
            // Set new content for InData Message
            FileNameSpec fs;
            ProductMetadata m;
            if (!fs.parseFileName(file, m)) {
                WarnMsg("Problem while trying to parse filename with regex");
                continue;
            }

            Message<MsgBodyINDATA> msg;
            msg.buildHdr(ChnlInData, MsgInData, "1.0",
                         compName, "*",
                         "", "", "");

            MsgBodyINDATA body;
            body["products"][0] = m.val();
            msg.buildBody(body);

            std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
            it = connections.find(ChnlInData);
            if (it != connections.end()) {
                ScalabilityProtocolRole * conn = it->second;
                conn->setMsgOut(msg.str());
            }
        }
    }

    if (((iteration + 1) == 100)) {
        Message<MsgBodyCMD> msg;
        MsgBodyCMD body;
        msg.buildHdr(ChnlCmd,
                     MsgCmd,
                     "1.0",
                     compName,
                     "*",
                     "", "", "");
        body["cmd"] = CmdInit;
        body["iteration"] = std::to_string(iteration);
        body["sessionId"] = cfg.sessionId;
        msg.buildBody(body);
        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        it = connections.find(ChnlCmd);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            conn->setMsgOut(msg.str());
        }
    }

    if ((iteration % 20) == 0) {
        // Create message and send
        Message<MsgBodyCMD> msg;
        MsgBodyCMD body;
        msg.buildHdr(ChnlCmd,
                     MsgCmd,
                     "1.0",
                     compName,
                     "*",
                     "", "", "");
        body["cmd"] = "PING";
        msg.buildBody(body);

        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        std::string chnl(ChnlHMICmd);
        it = connections.find(chnl);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            conn->setMsgOut(msg.str());
            InfoMsg("Sending request via channel " + chnl);
        }
    }

    if (iteration > 1000) {
        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        it = connections.find(ChnlCmd);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            json jdata;
            jdata["cmd"]       = "QUIT";
            jdata["iteration"] = std::to_string(iteration);
            JValue msg(jdata);
            conn->setMsgOut(msg.str());
        }

        transitTo(RUNNING);
    }
}

//----------------------------------------------------------------------
// Method: processHMICmdMsg
//----------------------------------------------------------------------
void EvtMng::processHMICmdMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    std::string cmd = JValue(m)["cmd"].asString();

    if (cmd == CmdStates) {
        // Create message and send
        Message<MsgBodyCMD> msg;
        MsgBodyCMD body;
        msg.buildHdr(ChnlHMICmd,
                     MsgHMICmd,
                     "1.0",
                     compName,
                     "*",
                     "", "", "");
        body["cmd"] = CmdStates;
        json states;
        for(auto & kv : cfg.nodeStates) {
            states[kv.first] = kv.second;
        }
        body["states"] = states;
        msg.buildBody(body);
        c->setMsgOut(msg.str());
    }
}

//----------------------------------------------------------------------
// Method: processTskRepDistMsg
//----------------------------------------------------------------------
void EvtMng::processTskRepDistMsg(ScalabilityProtocolRole* c, MessageString & m)
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
// Method: processHostMonMsg
//----------------------------------------------------------------------
void EvtMng::processHostMonMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    Message<MsgBodyTSK> msg(m);
    MsgBodyTSK & body = msg.body;
    JValue hostInfoData(body["info"]);

    HostInfo hostInfo;
    hostInfo.fromStr(hostInfoData.str());

    DBG(hostInfo.dump() + "\n");
}

//}
