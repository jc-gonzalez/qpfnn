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
#include "timer.h"

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
               AgentMode mode, const std::vector<std::string> & nds,
               ServiceInfo * srvInfo)
    : Component(name, addr, s), remote(true), agentMode(mode), nodes(nds),
      pStatus(IDLE), serviceInfo(srvInfo)
{
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
TskAge::TskAge(std::string name, std::string addr, Synchronizer * s,
               AgentMode mode, const std::vector<std::string> & nds,
               ServiceInfo * srvInfo)
    : Component(name, addr, s), remote(true), agentMode(mode), nodes(nds),
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
        idleCycles              =  0;
        maxWaitingCycles        = 20;
        idleCyclesBeforeRequest = 30;

        TRC("Agent Mode: CONTAINER");

    } else {

        // Create list of workers
        srvManager = nodes.at(0);
        nodes.erase(nodes.begin());

        TRC("Agent Mode: SERVICE  - " + srvManager);

        // Create Service Manager
        dckMng = new ServiceMng(srvManager, nodes);

        if (serviceInfo != 0) {
            // Create Service
            dckMng->createService(serviceInfo->service, serviceInfo->serviceImg,
                                  serviceInfo->scale,
                                  serviceInfo->exe, serviceInfo->args);
        }
    }

    numTask = 0;
    runningTask = 0;

    // Get initial values for Host Info structure
    hostInfo.hostIp = compAddress;
    hostInfo.cpuInfo.overallCpuLoad.timeInterval = 0;
    hostInfo.update();
    hostInfo.cpuInfo.overallCpuLoad.timeInterval = 10;

    armHostInfoTimer();

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
    // Declare status
    InfoMsg("Status is " + ProcStatusName[pStatus] +
            " at iteration " + str::toStr<int>(iteration));

    // Upon status, perform the required action
    switch (pStatus) {
    case IDLE:
        ++idleCycles;
        // Request task for processing in case the agent is idle
        if (idleCycles > idleCyclesBeforeRequest) {
            // Create message and send
            Message<MsgBodyTSK> msg;
            msg.buildHdr(ChnlTskProc, MsgTskRqst, CHNLS_IF_VERSION,
                         compName, "TskMng",
                         "", "", "");

            std::string chnl(ChnlTskProc + "_" + compName);
            send(chnl, msg.str());
            DBG("Sending request via channel " + chnl);
            InfoMsg("Sending request via channel " + chnl);

            pStatus = WAITING;
            InfoMsg("Switching to status " + ProcStatusName[pStatus]);
            waitingCycles = 0;
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
        if (runningTask != 0) {
            delete runningTask;
            runningTask = 0;
        }
        pStatus = IDLE;
        InfoMsg("Switching back to status " + ProcStatusName[pStatus]);
        idleCycles = 0;
        break;
    default:
        break;
    }

}

//----------------------------------------------------------------------
// Method: runEachIterationForServices
//----------------------------------------------------------------------
void TskAge::runEachIterationForServices()
{
    // Every N iterations, check if this is the first agent in the
    // host, and if it is update the hostInfo structure, and send it
    // to the TskMng
    
    //if ((iteration % 20) == 0) {
    //    sendHostInfoUpdate();
    //}
}

//----------------------------------------------------------------------
// Method: armHostInfoTimer
// Arm new timer for sending HostInfo updates
//----------------------------------------------------------------------
void TskAge::armHostInfoTimer()
{
    Timer * hstnfoSender = new Timer(6000, true,
                                     &TskAge::sendHostInfoUpdate, this);
}

//----------------------------------------------------------------------
// Method: processTskProcMsg
//----------------------------------------------------------------------
void TskAge::processTskProcMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    Message<MsgBodyTSK> msg(m);

    // Return if not recipient
    if (msg.header.target() != compName) { return; }

    if (pStatus == WAITING) {

        // Define and set task object
        MsgBodyTSK & body = msg.body;
        runningTask = new TaskInfo(body["info"]);
        TaskInfo & task = (*runningTask);

        task["taskHost"]  = compAddress;
        task["taskAgent"] = compName;

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
    while (!dckMng->getInfo(containerId, info)) {}

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

    // Put declared status in task info structure...
    task["taskStatus"] = taskStatus;
    // ... and add it as well to the taskData JSON structure
    task["taskData"]["State"]["TaskStatus"] = taskStatus;

    sendBodyElem<MsgBodyTSK>(ChnlTskProc,
                             ChnlTskProc + "_" + compName, MsgTskRep,
                             compName, "TskMng",
                             "info", task.str(),
                             origMsgString);
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

//----------------------------------------------------------------------
// Method: sendHostInfoUpdate
//----------------------------------------------------------------------
void TskAge::sendHostInfoUpdate()
{
    // Update host information
    hostInfo.update();

    sendBodyElem<MsgBodyTSK>(ChnlTskProc,
                             ChnlTskProc + "_" + compName, MsgHostMon,
                             compName, "TskMng",
                             "info", hostInfo.toJsonStr(), "");

    armHostInfoTimer();
}

//}
