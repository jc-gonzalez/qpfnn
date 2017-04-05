/******************************************************************************
 * File:    taskage.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.taskage
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
 *   Implement TaskAge class
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

#include "tskage.h"

#include "log.h"
#include "tools.h"
#include "channels.h"
#include "message.h"
#include "str.h"
#include "urlhdl.h"

#include "cntrmng.h"
#include "srvmng.h"

#include "config.h"

using Configuration::cfg;

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

#define T(x) #x
const std::string TskAge::ProcStatusName[] { TLIST_PSTATUS };
#undef T

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
TskAge::TskAge(const char * name, const char * addr, Synchronizer * s,
               AgentMode mode, ServiceInfo * srvInfo)
    : Component(name, addr, s), agentMode(mode),
      pStatus(IDLE), serviceInfo(srvInfo)
{
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
TskAge::TskAge(std::string name, std::string addr, Synchronizer * s,
               AgentMode mode, ServiceInfo * srvInfo)
    : Component(name, addr, s), agentMode(mode),
      pStatus(IDLE), serviceInfo(srvInfo)
{
}

//----------------------------------------------------------------------
// Method: fromRunningToOperational
//----------------------------------------------------------------------
void TskAge::fromRunningToOperational()
{
    if (agentMode == CONTAINER) {

        // Create Container Manager
        dckMng = new ContainerMng;

    } else {

        // Create list of workers
        srvWorkers = cfg.network.serviceNodes();
        srvManager = srvWorkers.at(0);
        srvWorkers.erase(srvWorkers.begin());

        // Create Service Manager
        dckMng = new ServiceMng(srvManager, srvWorkers);

        // Create Service
        dckMng->createService(serviceInfo->service, serviceInfo->serviceImg,
                              serviceInfo->scale,
                              serviceInfo->exe, serviceInfo->args);
    }

    transitTo(OPERATIONAL);
    InfoMsg("New state: " + getStateName(getState()));
}

//----------------------------------------------------------------------
// Method: runEachIteration
//----------------------------------------------------------------------
void TskAge::runEachIteration()
{
    InfoMsg("Status is " + ProcStatusName[pStatus] +
            " at iteration " + str::toStr<int>(iteration));

    // Request task for processing in case the agent is idle
    if ((pStatus == IDLE) && (iteration > 100)) {
        // Create message and send
        Message<MsgBodyTSK> msg;
        msg.buildHdr(ChnlTskRqst,
                     ChnlTskRqst,
                     "1.0",
                     compName,
                     "*",
                     "", "", "");

        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        std::string chnl(ChnlTskProc + "_" + compName);
        it = connections.find(chnl);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            conn->setMsgOut(msg.str());
            InfoMsg("Sending request via channel " + chnl);

        }

        pStatus = WAITING;
        InfoMsg("Switching to status " + ProcStatusName[pStatus]);
    }

    if (pStatus == FINISHING) {
        pStatus = IDLE;
        InfoMsg("Switching back to status " + ProcStatusName[pStatus]);
    }

    if ((pStatus == PROCESSING) && (iteration > 150)) {
        pStatus = FINISHING;
        InfoMsg("Switching to status " + ProcStatusName[pStatus]);
    }
}

//----------------------------------------------------------------------
// Method: processIncommingMessages
//----------------------------------------------------------------------
void TskAge::processIncommingMessages()
{
    MessageString m;
    TRC("TskAge::processIncommingmessages()");
    for (auto & kv: connections) {
        const ChannelDescriptor & chnl = kv.first;
        ScalabilityProtocolRole * conn = kv.second;
        while (conn->next(m)) {
            Message<MsgBodyTSK> msg(m);
            std::string type(msg.header.type());
            DBG(compName << " received the message [" << m << "] through the channel " + chnl);
            if      (chnl == ChnlCmd)     { processCmdMsg(conn, m); }
            else if (type == ChnlTskProc) { processTskProcMsg(conn, m); }
            else    { WarnMsg("Message from unidentified channel " + chnl); }
        }
    }
}

//----------------------------------------------------------------------
// Method: processCmdMsg
//----------------------------------------------------------------------
void TskAge::processCmdMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    MessageString ack = "ACK from " + compName;
    c->setMsgOut(ack);
}

//----------------------------------------------------------------------
// Method: processTskProcMsg
//----------------------------------------------------------------------
void TskAge::processTskProcMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    if (pStatus == WAITING) {
        // Define ans set task object
        Message<MsgBodyTSK> msg(m);
        MsgBodyTSK & body = msg.body;
        TaskInfo task(body["info"]);

        std::string agName(msg.header.source());
        DBG(">>>>>>>>>> RECEIVED TASK INFO FOR PROCESSING");
        DBG(">>>>>>>>>> name:" << msg.body("info")["name"].asString());

        // Retrieve the input products
        URLHandler urlh;
        int i = 0;
        for (auto & m : task.inputs.products) {
            urlh.setProduct(m);
            ProductMetadata & mg = urlh.fromGateway2Processing();

            task.inputs.products.push_back(mg);
            task["inputs"][i] = mg.val();
            ++i;
        }

        pStatus = PROCESSING;
    }
}

//}
