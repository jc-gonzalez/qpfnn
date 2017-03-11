/******************************************************************************
 * File:    evtmng.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.evtmng
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
EvtMng::EvtMng(const char * name, const char * addr, Synchronizer * s) : Component(name, addr, s)
{
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
EvtMng::EvtMng(std::string name, std::string addr, Synchronizer * s)  : Component(name, addr, s)
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
// Method: fromRunningToOff
//----------------------------------------------------------------------
void EvtMng::fromRunningToOff()
{
    InfoMsg("New state: " + getStateName(getState()));
    InfoMsg("Ending . . . ");

    synchro->notify();
}

//----------------------------------------------------------------------
// Method: processIncommingMessages
//----------------------------------------------------------------------
void EvtMng::processIncommingMessages()
{
    MessageString m;
    DBG("processIncommingmessages()");
    for (auto & kv: connections) {
        const ChannelDescriptor & chnl = kv.first;
        ScalabilityProtocolRole * conn = kv.second;
        DBG(". Checking channel " << chnl);
        while (conn->next(m)) {
            DBG("%%%%%%%%% " << compName << " received the message [" << m << "]");
            (*this.*msgProcMethods[chnl])(m);
        }
    }
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

            Message<MsgBodyINDATA> msg;
            msg.buildHdr(ChnlInData,
                         ChnlInData,
                         "1.0",
                         compName,
                         "DataMng",
                         "", "", "");

            MsgBodyINDATA body;
            JValue record("{\"" + prodType + "\": " + m.str() + "}");
            body["products"] = record.val();
            msg.buildBody(body);

            std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
            it = connections.find(ChnlInData);
            if (it != connections.end()) {
                ScalabilityProtocolRole * conn = it->second;
                conn->setMsgOut(msg.str());
            }


            /*
            // Create message and send it to appropriate targets
            std::array<std::string,1> fwdRecip = {"DataMng"};
            for (std::string & recip : fwdRecip) {
                MessageHeader hdr;
                buildMsgHeader(MSG_INDATA_IDX, "EvtMng", recip, hdr);

                Message_INDATA msg;
                buildMsgINDATA(hdr, products, msg);

                // Send message
                PeerMessage * pmsg = buildPeerMsg(hdr.destination, msg.getDataString(), MSG_INDATA);
                registerMsg(selfPeer()->name, *pmsg);
                setTransmissionToPeer(hdr.destination, pmsg);
                }*/

        }
    }
/*
    // 2. Check possible commands in DB
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);
    std::string cmdSource;
    std::string cmdContent;
    int cmdId;
    try {
        // Check that connection with the DB is possible
        dbHdl->openConnection();
        // Store new state
        dbHdl->getICommand(selfPeer()->name, cmdId, cmdSource, cmdContent);

        if (cmdContent == "QUIT") {
            InfoMsg("Leaving OPERATIONAL state triggered by an iCommand...");
            transitTo(RUNNING);
            // Mark command as executed
            dbHdl->markICommandAsDone(cmdId);
        } else if (cmdContent == "PING") {
            InfoMsg("QPFHMI requests answer. . .");
            dbHdl->removeICommand(cmdId);
            // Add answer command
            dbHdl->addICommand(cmdSource, selfPeer()->name, "PONG");
        }
    } catch (RuntimeException & e) {
        ErrMsg(e.what());
        return;
    }
    // Close connection
    dbHdl->closeConnection();
*/
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
    if (((iteration + 1) % 9) == 0) {
        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        it = connections.find(ChnlInData);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            char msg[128];
            sprintf(msg, "New incoming product at iter.# %d ...", iteration);
            conn->setMsgOut(msg);
        }
    }

    if (iteration > 1000) { transitTo(RUNNING); }
}

void EvtMng::processCmdMsg(MessageString & m)
{
    DBG("############# Processing CMD message!");
}

void EvtMng::processMonitMsg(MessageString & m)
{
    DBG("############# Processing MONIT message!");

}

void EvtMng::processTskRepMsg(MessageString & m)
{
    DBG("############# Processing TSKREP message!");

}


//}
