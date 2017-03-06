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
EvtMng::EvtMng(const char * name, const char * addr) : Component(name, addr)
{
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
EvtMng::EvtMng(std::string name, std::string addr)  : Component(name, addr)
{
}

//----------------------------------------------------------------------
// Method: fromInitialisedToRunning
//----------------------------------------------------------------------
void EvtMng::fromInitialisedToRunning()
{
    // Establish communications with other peer
    //establishCommunications();
    //while (waitingForGO) {}

    // Broadcast START message
    InfoMsg("Broadcasting START message . . .");
    //PeerMessage * startMsg = buildPeerMsg("", "Wake up!", MSG_START);
    //registerMsg(selfPeer()->name, *startMsg, true);
    //broadcast(startMsg);
}

//----------------------------------------------------------------------
// Method: fromRunningToOperational
//----------------------------------------------------------------------
void EvtMng::fromRunningToOperational()
{
    // Install DirWatcher at inbox folder
    dw = new DirWatcher(Config::PATHBase + "/data/inbox");
}

//----------------------------------------------------------------------
// Method: fromOperationalToRunning
//----------------------------------------------------------------------
void EvtMng::fromOperationalToRunning()
{
    // stop watching inbox folder
    dw->stop();

    // Broadcast STOP message
    InfoMsg("Broadcasting STOP message . . .");
    //PeerMessage * stopMsg = buildPeerMsg("", "Shut down!", MSG_STOP);
    //registerMsg(selfPeer()->name, *stopMsg, true);
    //broadcast(stopMsg);
}

//----------------------------------------------------------------------
// Method: fromRunningToOff
//----------------------------------------------------------------------
void EvtMng::fromRunningToOff()
{
    InfoMsg("Ending . . . ");
}

//----------------------------------------------------------------------
// Method: execAdditonalLoopTasks
//----------------------------------------------------------------------
void EvtMng::execAdditonalLoopTasks()
{
    // Check DirWatcher events from inbox folder
    DirWatcher::DirWatchEvent e;
    while (dw->nextEvent(e)) {
        std::cout << e.path << "/" << e.name << (e.isDir ? " DIR " : " ") << e.mask << std::endl;

        // Process only files
        // TODO: Process directories that appear at inbox
        if (! e.isDir) {
            /*
            std::string file(e.path + "/" + e.name);
            // Set new content for InData Message
            FileNameSpec fs;
            ProductMetadata m;
            fs.parseFileName(file, m);
            ProductCollection products;
            products.productList[m.productType] = m;

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
            }
            */
        }
    }

    // 2. Check possible commands in DB
    /*
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
}

//----------------------------------------------------------------------
// Method: runEachIteration
//----------------------------------------------------------------------
void EvtMng::runEachIteration()
{
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
}

//}
