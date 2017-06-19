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

#include <dirent.h>

#include "cntrmng.h"
#include "srvmng.h"

#include "filenamespec.h"

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

        // Set parameters for requesting tasks and waiting
        idleCycles              = 0;
        maxWaitingCycles        = 20;
        idleCyclesBeforeRequest = 30;

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

    // Get initial values for Host Info structure
    hostInfo.cpuInfo.overallCpuLoad.timeInterval = 0;
    hostInfo.update();
    hostInfo.cpuInfo.overallCpuLoad.timeInterval = 10;

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

    switch (pStatus) {
    case IDLE:
        ++idleCycles;
        // Request task for processing in case the agent is idle
        if (idleCycles > idleCyclesBeforeRequest) {
            // Create message and send
            Message<MsgBodyTSK> msg;
            msg.buildHdr(ChnlTskProc, MsgTskRqst, "1.0",
                         compName, "TskMng",
                         "", "", "");

            std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
            std::string chnl(ChnlTskProc + "_" + compName);
            DBG("Looking for channel " << chnl);
            it = connections.find(chnl);
            if (it != connections.end()) {
                ScalabilityProtocolRole * conn = it->second;
                conn->setMsgOut(msg.str());
                DBG("Sending request via channel " + chnl);
                InfoMsg("Sending request via channel " + chnl);
                pStatus = WAITING;
                InfoMsg("Switching to status " + ProcStatusName[pStatus]);
                waitingCycles = 0;
            }
        }
        break;
    case WAITING:
        ++waitingCycles;
        if (waitingCycles > maxWaitingCycles) {
            pStatus = IDLE;
            InfoMsg("Switching back to status " + ProcStatusName[pStatus]);
            idleCycles = 0;
        }
        break;
    case PROCESSING:
        sendTaskReport();
        break;
    case FINISHING:
        pStatus = IDLE;
        InfoMsg("Switching back to status " + ProcStatusName[pStatus]);
        idleCycles = 0;
        break;
    default:
        break;
    }

    if (compName.substr(compName.size() - 3) == "_01") {
        if ((iteration % 10) == 0) {
            hostInfo.update();
        }
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
            else if ((type == MsgTskProc) &&
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
    json msg(m);
    std::string sessId = msg["sessionId"].asString();
    if (sessId != cfg.sessionId) {
        cfg.synchronizeSessionId(sessId);
    }

    MessageString ack = "ACK from " + compName;
    c->setMsgOut(ack);
}

//----------------------------------------------------------------------
// Method: processTskProcMsg
//----------------------------------------------------------------------
void TskAge::processTskProcMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    if (pStatus == WAITING) {

        // Define and set task object
        Message<MsgBodyTSK> msg(m);
        MsgBodyTSK & body = msg.body;
        runningTask = new TaskInfo(body["info"]);
        TaskInfo & task = (*runningTask);

        assert(compName == msg.header.target());
        DBG(">>>>>>>>>> " << compName
            << " RECEIVED TASK INFO FOR PROCESSING\n"
            ">>>>>>>>>> Task name:" << msg.body("info")["name"].asString());

        numTask++;

        //---- Define processing environment
        std::string sessId = task.taskSession();
        DBG(">> [" << sessId << "] vs. [" << cfg.sessionId << "]");
        if (sessId != cfg.sessionId) {
            DBG(">> CHANGING SESSION ID");
            str::replaceAll(workDir, cfg.sessionId, sessId);
            cfg.synchronizeSessionId(sessId);
        }

        //---- Create exchange area
        internalTaskNameIdx = (compName + "-" + timeTag() + "-" +
                               std::to_string(numTask));

        exchangeDir = workDir + "/" + internalTaskNameIdx;
        exchgIn     = exchangeDir + "/in";
        exchgOut    = exchangeDir + "/out";
        exchgLog    = exchangeDir + "/log";

        mkdir(exchangeDir.c_str(), 0755);
        mkdir(exchgIn.c_str(),     0755);
        mkdir(exchgOut.c_str(),    0755);
        mkdir(exchgLog.c_str(),    0755);

        //---- Retrieve the input products
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

        //----  * * * LAUNCH TASK * * *
        if (dckMng->createContainer(task.taskPath(), exchangeDir, containerId)) {

            InfoMsg("Running task " + task.taskName() +
                    " (" + task.taskPath() + ") within container " + containerId);
            origMsgString = m;
            sleep(1);

            // Set processing status
            pStatus = PROCESSING;
            workingDuring = 0;

            // Send back information to Task Manager
            sendTaskReport();

        } else {
            WarnMsg("Couldn't execute docker container");
        }

    }
}

//----------------------------------------------------------------------
// Method: sendTaskReport
//----------------------------------------------------------------------
void TskAge::sendTaskReport()
{
    // Define and set task object
    TaskInfo & task = (*runningTask);

    std::stringstream info;
    if (dckMng->getInfo(containerId, info)) {

        JValue jinfo(info.str());
        json taskData = jinfo.val()[0];
        task["taskData"] = taskData;

        json jstate = taskData["State"];
        std::string inspStatus = jstate["Status"].asString();
        int         inspCode   = jstate["ExitCode"].asInt();

        if      (inspStatus == "running") {
            taskStatus = TASK_RUNNING;
        } else if (inspStatus == "paused") {
            taskStatus = TASK_PAUSED;
        } else if (inspStatus == "created") {
            taskStatus = TASK_STOPPED;
        } else if (inspStatus == "dead") {
            taskStatus = TASK_STOPPED;
        } else if (inspStatus == "exited") {
            taskStatus = (inspCode == 0) ? TASK_FINISHED : TASK_FAILED;
        } else {
            taskStatus = TASK_UNKNOWN_STATE;
        }

    } else {
        taskStatus = TASK_FAILED;
    }

    if ((taskStatus == TASK_STOPPED) ||
        (taskStatus == TASK_FAILED) ||
        (taskStatus == TASK_FINISHED) ||
        (taskStatus == TASK_UNKNOWN_STATE)) {
        InfoMsg("Task container monitoring finished");
        pStatus = FINISHING;
        InfoMsg("Switching to status " + ProcStatusName[pStatus]);
    } else {
        workingDuring++;
    }

    if (taskStatus == TASK_FINISHED) {
        retrieveOutputProducts();
    }

    task["taskStatus"] = taskStatus;

    // Define and set task object
    Message<MsgBodyTSK> msg(origMsgString);
    MsgBodyTSK & body = msg.body;
    body["info"] = task.val();

    msg.buildHdr(ChnlTskProc, MsgTskRep, "1.0",
                 compName, "TskMng",
                 "", "", "");
    msg.buildBody(body);

    // Send msg
    std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
    std::string chnl(ChnlTskProc + "_" + compName);
    it = connections.find(chnl);
    if (it != connections.end()) {
        ScalabilityProtocolRole * conn = it->second;
        TRC("Sending report <<" + msg.str() + ">>");
        conn->setMsgOut(msg.str());
    }

}

//----------------------------------------------------------------------
// Method: retrieveOutputProducts
//----------------------------------------------------------------------
void TskAge::retrieveOutputProducts()
{
    // Define and set task object
    TaskInfo & task = (*runningTask);

    DBG("Retrieving output products for task: " << task.taskName());

    //-------------------------------------------------------------------
    // Get output data
    //-------------------------------------------------------------------
    std::vector<std::string> outFiles;
    DIR * dp = NULL;
    struct dirent * dirp;
    for (auto & vd : {exchgOut, exchgLog}) {
        if ((dp = opendir(vd.c_str())) == NULL) {
            WarnMsg("Cannot open output directory " + vd);
            TRC("Cannot open output directory " + vd);
        } else {
            while ((dirp = readdir(dp)) != NULL) {
                if (dirp->d_name[0] != '.') {
                    std::string dname(dirp->d_name);
                    //if (dname.substr(0, 3) != "EUC") { continue; }
                    std::string outputFile = vd + "/" + dname;
                    TRC("Saving outfile " + outputFile);
                    outFiles.push_back(outputFile);
                }
            }
            closedir(dp);
        }
    }

    TRC("outFiles has " << outFiles.size() << " elements");
    task.outputs.products.clear();

    FileNameSpec fs;
    for (unsigned int i = 0; i < outFiles.size(); ++i) {
        ProductMetadata m;
        if (fs.parseFileName(outFiles.at(i), m, ProcessingSpace, task.taskPath())) {
            // Place output product at external (output) shared area
            urlh.setProduct(m);
            m = urlh.fromProcessing2Gateway();
        } else {
            continue;
        }
        task.outputs.products.push_back(m);
        task["outputs"][i] = m.val();
    }
}

//}
