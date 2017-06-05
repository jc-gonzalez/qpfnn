/******************************************************************************
 * File:    taskage.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.taskage
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
    : Component(name, addr, s), remote(true), agentMode(mode),
      pStatus(IDLE), serviceInfo(srvInfo)
{
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
TskAge::TskAge(std::string name, std::string addr, Synchronizer * s,
               AgentMode mode, ServiceInfo * srvInfo)
    : Component(name, addr, s), remote(true), agentMode(mode),
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

    numTask = 0;

    transitTo(OPERATIONAL);
    InfoMsg("New state: " + getStateName(getState()));
}

//----------------------------------------------------------------------
// Method: runEachIteration
//----------------------------------------------------------------------
void TskAge::runEachIteration()
{
    if (agentMode == CONTAINER) {
        runEachIterationForContainers();
    } else {
        runEachIterationForServices();
    }
}

//----------------------------------------------------------------------
// Method: runEachIterationForContainers
//----------------------------------------------------------------------
void TskAge::runEachIterationForContainers()
{
    InfoMsg("Status is " + ProcStatusName[pStatus] +
            " at iteration " + str::toStr<int>(iteration));

    // Request task for processing in case the agent is idle
    if ((pStatus == IDLE) && (iteration == 100)) {
        // Create message and send
        Message<MsgBodyTSK> msg;
        msg.buildHdr(ChnlTskRqst, ChnlTskRqst, "1.0",
                     compName, "*",
                     "", "", "");

        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        std::string chnl(ChnlTskProc + "_" + compName);
        it = connections.find(chnl);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            conn->setMsgOut(msg.str());
            InfoMsg("Sending request via channel " + chnl);
            pStatus = WAITING;
            InfoMsg("Switching to status " + ProcStatusName[pStatus]);
        }
    }

    if (pStatus == FINISHING) {
        pStatus = IDLE;
        InfoMsg("Switching back to status " + ProcStatusName[pStatus]);
    }

    if ((pStatus == PROCESSING) && (iteration == 120)) {
        pStatus = FINISHING;
        InfoMsg("Switching to status " + ProcStatusName[pStatus]);
    }
}

//----------------------------------------------------------------------
// Method: runEachIterationForServices
//----------------------------------------------------------------------
void TskAge::runEachIterationForServices()
{
}

//----------------------------------------------------------------------
// Method: processIncommingMessages
//----------------------------------------------------------------------
void TskAge::processIncommingMessages()
{
    MessageString m;
    TRC("TskAge::processIncommingmessages() while status is " +
        ProcStatusName[pStatus] +
        " at iteration " + std::to_string(iteration));
    for (auto & kv: connections) {
        const ChannelDescriptor & chnl = kv.first;
        ScalabilityProtocolRole * conn = kv.second;
        while (conn->next(m)) {
            Message<MsgBodyTSK> msg(m);
            std::string type(msg.header.type());
            DBG(compName << " received the message [" << m
                << "] through the channel " + chnl);
            if      (chnl == ChnlCmd)     { processCmdMsg(conn, m); }
            else if ((type == ChnlTskProc) &&
                     (msg.header.target() == compName)) { processTskProcMsg(conn, m); }
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

        assert(compName == msg.header.target());

        DBG(">>>>>>>>>> " << compName
            << " RECEIVED TASK INFO FOR PROCESSING\n"
            ">>>>>>>>>> Task name:" << msg.body("info")["name"].asString());

        numTask++;

        //............................................................
        // Define processing environment

        // Prepare folders:
        // * workDir             := <DIR>/qpf/run/yymmddTHHMMSS/tsk
        // * internalTaskNameIdx := TskAgentName-yyyymmddTHHMMSS-n
        // * exchangeDir         := workDir + / + TskAgent1-yyyymmddTHHMMSS-n
        internalTaskNameIdx = (compName + "-" + timeTag() + "-" +
                               std::to_string(numTask));
        exchangeDir = workDir + "/" + internalTaskNameIdx;
        exchgIn     = exchangeDir + "/in";
        exchgOut    = exchangeDir + "/out";
        exchgLog    = exchangeDir + "/log";

        // Create exchange area
        mkdir(exchangeDir.c_str(), 0755);
        mkdir(exchgIn.c_str(),     0755);
        mkdir(exchgOut.c_str(),    0755);
        mkdir(exchgLog.c_str(),    0755);

        // Define task parameters
        std::string sysBinDir  = sysDir + "/bin";
        std::string taskDriver = sysDir + "/bin/runTask.sh";
        std::string cfgFile    = exchangeDir + "/dummy.cfg";

        //............................................................
        // Retrieve the input products
        URLHandler urlh;
        urlh.setProcElemRunDir(workDir, internalTaskNameIdx);
        if (remote) {
            urlh.setRemoteCopyParams(cfg.network.masterNode(), compAddress);
        }

        int i = 0;
        for (auto & m : task.inputs.products) {
            urlh.setProduct(m);
            ProductMetadata & mg = urlh.fromGateway2Processing();

            task.inputs.products.push_back(mg);
            task["inputs"][i] = mg.val();
            ++i;
        }

        //............................................................
        // * * * LAUNCH TASK * * *

        static const std::string QPFImgRun("/qpf/run");

        std::string containerId;

        std::string taskDir = QPFImgRun + "/" + internalTaskNameIdx;
        std::map<std::string, std::string> dirMaps;
        dirMaps[exchangeDir] = taskDir;
        //dirMaps[exchgIn]     = taskDir + "/in";
        //dirMaps[exchgOut]    = taskDir + "/out";

        link("/home/eucops/qpf/run/bin/DummyProc",
             (taskDir + "/DummyProc").c_str());
        bool dckExec = dckMng->createContainer("qla-debian:a",
                                               {"-d", "-P", "--privileged=true", "-w=" + taskDir},
                                               dirMaps,
                                               "/bin/bash",
                                               {"DummyProc"},
                                               containerId);

        std::stringstream info;

        if (dckExec) {
            sleep(1);
            if (dckMng->getInfo(containerId, info)) {
                InfoMsg(info.str());
            }
        } else {
            WarnMsg("Couldn't execute docker container");
        }

        // Set processing status
        pStatus = PROCESSING;
    }
}

//}
