/******************************************************************************
 * File:    taskmng.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.taskmng
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
 *   Implement TskMng class
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

#include "tskmng.h"

#include <sys/time.h>
#include <array>

#include "channels.h"
#include "str.h"
#include "log.h"
#include "tools.h"
#include "config.h"
#include "timer.h"

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
TskMng::TskMng(const char * name, const char * addr, Synchronizer * s)
    : Component(name, addr, s)
{
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
TskMng::TskMng(std::string name, std::string addr, Synchronizer * s)
    : Component(name, addr, s)
{
}

//----------------------------------------------------------------------
// Method: fromRunningToOperational
//----------------------------------------------------------------------
void TskMng::fromRunningToOperational()
{
    // Create Agents Info. table
    AgentInfo emptyInfo;
    for (auto & a : cfg.agentNames) {
        agents.push_back(a);
        emptyInfo.name = a;
        emptyInfo.load = 0.0;
        agentInfo[a] = emptyInfo;
    }

    // Initialize Task Status maps
    for (int k = TASK_SCHEDULED; k != TASK_UNKNOWN_STATE; ++k) {
        TaskStatus status = TaskStatus(k);
        serviceTaskStatus[status]   = 0;
        containerTaskStatus[status] = 0;
        for (auto & a : cfg.agentNames) {
            std::pair<std::string,
                      TaskStatus> tskPair = std::make_pair(a, status);
            containerTaskStatusPerAgent[tskPair] = 0;
        }
    }

    // Launch the HTTP Server for Docker Swarm Services, and to
    // serve information through HTML pages. We activate the service to
    // provide data files only if we do really need it (swarm services)
    httpSrv = new HttpServer("httpSrv",
                             "cfg.network.httpServerAddress()",
                             cfg.network.serviceNodes().size() > 0);

    // Deactivate sending ProcessingFrameworkInfo updates
    sendingPeriodicFmkInfo = false;

    // Transit to Operational
    transitTo(OPERATIONAL);
    InfoMsg("New state: " + getStateName(getState()));
}

//----------------------------------------------------------------------
// Method: processIncommingMessages
//----------------------------------------------------------------------
void TskMng::processIncommingMessages()
{
    MessageString m;
    //TRC("TskMng::processIncommingmessages()");
    for (auto & kv: connections) {
        const ChannelDescriptor & chnl = kv.first;
        ScalabilityProtocolRole * conn = kv.second;
        while (conn->next(m)) {
            Message<MsgBodyTSK> msg(m);
            std::string type(msg.header.type());
            DBG(compName << " received the message of type " << type
                << " through the channel " + chnl);
            if      (chnl == ChnlCmd)      { processCmdMsg(conn, m); }
            else if (chnl == ChnlTskSched) { processTskSchedMsg(conn, m); }
            else if (type == MsgTskRqst)   { processTskRqstMsg(conn, m); }
            else if (type == MsgTskRep)    { processTskRepMsg(conn, m); }
            else if (type == MsgHostMon)   { processHostMonMsg(conn, m); }
            else    { WarnMsg("Message from unidentified channel " + chnl); }
        }
    }
}

//----------------------------------------------------------------------
// Method: runEachIteration
//----------------------------------------------------------------------
void TskMng::runEachIteration()
{
    // Each iteration the Task Manager, apart from processing incoming
    // messages, performs the following actions:
    // 1. Send Task Status Reports to

    TRC("SWARM: Q(" + std::to_string(serviceTasks.size()) + ")" +
        "R(" + std::to_string(serviceTaskStatus[TASK_RUNNING]) + ")" +
        "W(" + std::to_string(serviceTaskStatus[TASK_SCHEDULED]) + ")" +
        "P(" + std::to_string(serviceTaskStatus[TASK_PAUSED]) + ")" +
        "S(" + std::to_string(serviceTaskStatus[TASK_STOPPED]) + ")" +
        "E(" + std::to_string(serviceTaskStatus[TASK_FAILED]) + ")" +
        "F(" + std::to_string(serviceTaskStatus[TASK_FINISHED]) + ")" +
        "   CONT.: Q(" + std::to_string(containerTasks.size()) + ")" +
        "R(" + std::to_string(containerTaskStatus[TASK_RUNNING]) + ")" +
        "W(" + std::to_string(containerTaskStatus[TASK_SCHEDULED]) + ")" +
        "P(" + std::to_string(containerTaskStatus[TASK_PAUSED]) + ")" +
        "S(" + std::to_string(containerTaskStatus[TASK_STOPPED]) + ")" +
        "E(" + std::to_string(containerTaskStatus[TASK_FAILED]) + ")" +
        "F(" + std::to_string(containerTaskStatus[TASK_FINISHED]) + ")"
        );
}

//----------------------------------------------------------------------
// Method: processCmdMsg
//----------------------------------------------------------------------
void TskMng::processCmdMsg(ScalabilityProtocolRole * conn, MessageString & m)
{
    JValue msg(m);
    std::string cmd = msg["cmd"].asString();

    if (cmd == "QUIT") {
        transitTo(RUNNING);
    }
}

//----------------------------------------------------------------------
// Method: processTskSchedMsg
//----------------------------------------------------------------------
void TskMng::processTskSchedMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    // Define ans set task objecte
    Message<MsgBodyTSK> msg(m);
    MsgBodyTSK & body = msg.body;
    TaskInfo task(body["info"]);

    // Store task in specific container
    if (task.taskSet() == "CONTAINER") {
        containerTasks.push_back(task);
    } else if (task.taskSet() == "SERVICE") {
        serviceTasks.push_back(task);
    } else {
        WarnMsg("Badly assigned set for task: " + task.taskSet());
    }
}

//----------------------------------------------------------------------
// Method: processTskRqstMsg
//----------------------------------------------------------------------
void TskMng::processTskRqstMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    // Define and set task object
    Message<MsgBodyTSK> msg(m);
    std::string agName(msg.header.source());
    DBG("TASK REQUEST FROM " << agName << " RECEIVED");

    // Create message
    msg.buildHdr(ChnlTskProc, MsgTskProc, "1.0",
                 compName, agName, "", "", "");

    MsgBodyTSK body;

    // Select task to send
    bool isSrvRqst = (agName == "TskAgentSwarm");
    std::list<TaskInfo> * listOfTasks = (isSrvRqst) ? &serviceTasks : &containerTasks;

    bool isTaskSent = false;
    std::string taskName;
    TaskStatus  taskStatus;

    TRC("Pool of tasks has size of " << listOfTasks->size());

    if (listOfTasks->size() > 0) {
        json taskInfoData = listOfTasks->front().val();
        taskName = ((isSrvRqst ? "Swarm" : agName) + "_" +
                    taskInfoData["taskName"].asString());
        taskInfoData["taskName"] = taskName;
        taskStatus = TaskStatus(taskInfoData["taskStatus"].asInt());
        body["info"] = taskInfoData;
        listOfTasks->pop_front();

        msg.buildBody(body);

        // Send msg
        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        ChannelDescriptor chnl(ChnlTskProc + "_" + agName);
        it = connections.find(chnl);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            conn->setMsgOut(msg.str());
            isTaskSent = true;
        }
        TRC("Task " + taskName + "sent to " << agName);
    }

    // Task info is sent, register the task and status
    if (isTaskSent) {
        taskRegistry[taskName] = taskStatus;
        if (isSrvRqst) {
            serviceTaskStatus[taskStatus]++;
        } else {
            containerTaskStatus[taskStatus]++;
            std::pair<std::string, TaskStatus> tskPair =
                std::make_pair(taskName, taskStatus);
            containerTaskStatusPerAgent[tskPair]++;
        }
    }
}

//----------------------------------------------------------------------
// Method: processTskRepMsg
//----------------------------------------------------------------------
void TskMng::processTskRepMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    Message<MsgBodyTSK> msg(m);
    MsgBodyTSK & body = msg.body;
    TaskInfo task(body["info"]);

    std::string taskName  = task.taskName();
    std::string agName    = task.taskAgent();

    TaskStatus taskStatus = TaskStatus(task.taskStatus());
    TaskStatus oldStatus  = taskRegistry[taskName];

    TRC("Processing TaskReport: status: " << TaskStatusName[oldStatus] <<
        " ==> " << TaskStatusName[taskStatus]);

    // Update registry and status maps if needed
    if (oldStatus != taskStatus) {
        taskRegistry[taskName] = taskStatus;
        if (task.taskSet() == "SERVICE") {
            serviceTaskStatus[oldStatus]--;
            serviceTaskStatus[taskStatus]++;
        } else {
            containerTaskStatus[oldStatus]--;
            containerTaskStatus[taskStatus]++;
            std::pair<std::string, TaskStatus> oldPair =
                std::make_pair(agName, oldStatus);
            std::pair<std::string, TaskStatus> tskPair =
                std::make_pair(agName, taskStatus);
            containerTaskStatusPerAgent[oldPair]--;
            containerTaskStatusPerAgent[tskPair]++;

            TaskStatusSpectra spec = convertTaskStatusToSpectra(agName);

            const std::string & hostIp = task.taskHost();
            ProcessingHostInfo * procHostInfo = Config::procFmkInfo->hostsInfo[hostIp];
            for (auto & agi : procHostInfo->agInfo) {
                if (agi.name == hostIp) { agi.taskStatus = spec; }
            }
        }
    }

    if (taskStatus == TASK_FINISHED) {
        InfoMsg("Finished task " + taskName);
    }

    sendTskRepDistMsg(m, MsgTskRepDist);
}

//----------------------------------------------------------------------
// Method: processHostMonMsg
//----------------------------------------------------------------------
void TskMng::processHostMonMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    // Place new information in general structure
    consolidateMonitInfo(m);
    //sendTskRepDistMsg(m, MsgHostMon);

    // If sending updates is not yet activated, activate it
    if (!sendingPeriodicFmkInfo) {
        sendingPeriodicFmkInfo = true;
        armProcFmkInfoMsgTimer();
    }
}

//----------------------------------------------------------------------
// Method: armProcFmkInfoMsgTimer
// Arm new timer for sending ProcessingFrameworkInfo updates
//----------------------------------------------------------------------
void TskMng::armProcFmkInfoMsgTimer()
{
    Timer * fmkSender = new Timer(5000, true,
                                  &TskMng::sendProcFmkInfoUpdate, this);
}

//----------------------------------------------------------------------
// Method: convertTaskStatusToSpectra
// Convert set of status for an agent to a spectra tuple
//----------------------------------------------------------------------
TaskStatusSpectra TskMng::convertTaskStatusToSpectra(std::string & agName)
{
    TaskStatusSpectra spec;

    /*auto portnum = [](int start, int h, int i) -> int
        { return start + 10 * (h - 1) + i; };
    */
    std::map<std::pair<std::string, TaskStatus>, int>::iterator
        nonValid = containerTaskStatusPerAgent.end();
    std::map<std::pair<std::string, TaskStatus>, int>::iterator
        it;

    it = containerTaskStatusPerAgent.find(std::make_pair(agName, TASK_SCHEDULED));
    spec.scheduled = (it == nonValid) ? 0 : it->second;

    it = containerTaskStatusPerAgent.find(std::make_pair(agName, TASK_RUNNING));
    spec.running   = (it == nonValid) ? 0 : it->second;

    it = containerTaskStatusPerAgent.find(std::make_pair(agName, TASK_PAUSED));
    spec.paused    = (it == nonValid) ? 0 : it->second;

    it = containerTaskStatusPerAgent.find(std::make_pair(agName, TASK_STOPPED));
    spec.stopped   = (it == nonValid) ? 0 : it->second;

    it = containerTaskStatusPerAgent.find(std::make_pair(agName, TASK_FAILED));
    spec.failed    = (it == nonValid) ? 0 : it->second;

    it = containerTaskStatusPerAgent.find(std::make_pair(agName, TASK_FINISHED));
    spec.finished  = (it == nonValid) ? 0 : it->second;

    spec.sum();

    return spec;
}

//----------------------------------------------------------------------
// Method: execTask
// Execute the rule requested by Task Orchestrator
//----------------------------------------------------------------------
void TskMng::execContainerTask()
{
}

inline double weightFunc(double load, double tasks) {
    return 100 * load + tasks;
}

//----------------------------------------------------------------------
// Method: selectAgent
// Send a TaskProcessingMsg to the Task Manager, requesting the
// execution of a rule
//----------------------------------------------------------------------
std::string TskMng::selectAgent()
{
    return std::string();
}

//----------------------------------------------------------------------
// Method: consolidateMonitInfo
// Consolidates the monitoring info retrieved from the processing hosts
//----------------------------------------------------------------------
void TskMng::consolidateMonitInfo(MessageString & m)
{
    Message<MsgBodyTSK> msg(m);
    MsgBodyTSK & body = msg.body;
    JValue hostInfoData(body["info"]);

    HostInfo hostInfo;
    hostInfo.fromStr(hostInfoData.str());
    std::string hostIp = hostInfo.hostIp;

    ProcessingHostInfo* procHostInfo = Config::procFmkInfo->hostsInfo[hostIp];
    procHostInfo->hostInfo = hostInfo;
}

//----------------------------------------------------------------------
// Method: sendTaskAgMsg
// Send a TaskProcessingMsg to the Task Manager, requesting the
// execution of a rule
//----------------------------------------------------------------------
bool TskMng::sendTaskAgMsg(MessageString & m,
                           std::string agName)
{
    return true;
}

//----------------------------------------------------------------------
// Method: sendTskRepDistMsg
// Send a HostInfo message to EvtMng/QPFHMI/DataMng
//----------------------------------------------------------------------
bool TskMng::sendTskRepDistMsg(MessageString & m, const MessageDescriptor & msgType)
{
    bool retVal = false;
    // Set message header
    Message<MsgBodyTSK> msg(m);
    msg.buildHdr(ChnlTskRepDist, msgType, "1.0",
                 compName, "*", "", "", "");
    // Send msg
    std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
    ChannelDescriptor chnl(ChnlTskRepDist);
    it = connections.find(chnl);
    if (it != connections.end()) {
        ScalabilityProtocolRole * conn = it->second;
        conn->setMsgOut(msg.str());
        retVal = true;
    }
    return retVal;
}

//----------------------------------------------------------------------
// Method: sendProcFmkInfoUpdate
// Send an update on the ProcessingFrameworkInfo structure
//----------------------------------------------------------------------
void TskMng::sendProcFmkInfoUpdate()
{
    // Prepare message and send it
    Message<MsgBodyTSK> msg;
    MsgBodyTSK & body = msg.body;

    JValue fmkInfoValue(Config::procFmkInfo->toJsonStr());
    body["info"] = fmkInfoValue.val();

    // Set message header
    msg.buildHdr(ChnlTskRepDist, MsgFmkMon, "1.0",
                 compName, "*", "", "", "");
    // Send msg
    std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
    ChannelDescriptor chnl(ChnlTskRepDist);
    it = connections.find(chnl);
    if (it != connections.end()) {
        ScalabilityProtocolRole * conn = it->second;
        conn->setMsgOut(msg.str());
    } else {
        ErrMsg("Couldn't send updated ProcessingFrameworkInfo data.");
    }

    // Arm new timer
    if (sendingPeriodicFmkInfo) { armProcFmkInfoMsgTimer(); }
}


//}
