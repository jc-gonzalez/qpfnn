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
    InfoMsg("DirWatcher installed at " + Config::PATHBase + "/data/inbox");

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
        std::cout << e.path << "/" << e.name << (e.isDir ? " DIR " : " ") << e.mask << std::endl;

        // Process only files
        // TODO: Process directories that appear at inbox
        if (! e.isDir) {
            std::string file(e.path + "/" + e.name);
            // Set new content for InData Message
            FileNameSpec fs;
            ProductMetadata m;
            fs.parseFileName(file, m);
            /*
            m["productType"] = "LE1_VIS";
            m["instrument"] = "VIS";
            m["mission"] = "EUC";
            m["obsMode"] = "W";
            m["params"] = "";
            m["procFunc"] = "LE1";
            m["productVersion"] = "01.00";
            m["startTime"] = "20200101T000000.0Z";
            m["endTime"] = "20200101T120000.0Z";
            m["productId"] = "EUC_LE1_VIS-W-00034-1_20200707T144425.0Z_03.04";
            m["signature"] = "00034-W-1";

            std::string prodType(m.productType());
            */
            Message<MsgBodyINDATA> msg;
            msg.buildHdr(ChnlInData,
                         ChnlInData,
                         "1.0",
                         compName,
                         "*",
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

    if (((iteration + 1) % 10) == 0) {
        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        it = connections.find(ChnlCmd);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            char msg[128];
            sprintf(msg, "Tell me your name for iter.# %d ...", iteration);
            conn->setMsgOut(msg);
        }
    }

    if (iteration > 1000) { transitTo(RUNNING); }
}

//----------------------------------------------------------------------
// Method: processCmdMsg
//----------------------------------------------------------------------
void EvtMng::processCmdMsg(ScalabilityProtocolRole* c, MessageString & m)
{
}

//----------------------------------------------------------------------
// Method: processCmdMsg
//----------------------------------------------------------------------
void EvtMng::processHMICmdMsg(ScalabilityProtocolRole* c, MessageString & m)
{
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
        InfoMsg("Sending response via channel " + chnl);
    }
}

//----------------------------------------------------------------------
// Method: processTskRepMsg
//----------------------------------------------------------------------
void EvtMng::processTskRepMsg(ScalabilityProtocolRole* c, MessageString & m)
{
}

//}
