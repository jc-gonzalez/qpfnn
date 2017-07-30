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
            TaskStatusPerAgent tskPair = std::make_pair(a, status);
            containerTaskStatusPerAgent[tskPair] = 0;
        }
    }

    // Launch the HTTP Server for Docker Swarm Services, and to
    // serve information through HTML pages. We activate the service to
    // provide data files only if we do really need it (swarm services)
    httpSrv = new HttpServer("httpSrv",
                             "cfg.network.httpServerAddress()",
                             cfg.network.swarms().size() > 0);

    // Deactivate sending ProcessingFrameworkInfo updates
    sendingPeriodicFmkInfo = false;

    // Transit to Operational
    transitTo(OPERATIONAL);
    InfoMsg("New state: " + getStateName(getState()));
}

//----------------------------------------------------------------------
// Method: runEachIteration
//----------------------------------------------------------------------
void TskMng::runEachIteration()
{
    // Each iteration the Task Manager, apart from processing incoming
    // messages, performs the following actions:
    // 1. Send Task Status Reports to
    static std::string lastTrace;

    std::string trace = ("SWARM: Q(" + std::to_string(serviceTasks.size()) + ")" +
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
    if (trace != lastTrace) {
        TRC(trace);
        lastTrace = trace;
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
    msg.buildHdr(ChnlTskProc, MsgTskProc, CHNLS_IF_VERSION,
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
        taskStatus = TASK_SCHEDULED; //TaskStatus(taskInfoData["taskStatus"].asInt());
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
    }

    // Task info is sent, register the task and status
    if (isTaskSent) {
        TRC("Task " + taskName + "sent to " << agName);
        taskRegistry[taskName] = taskStatus;
        if (isSrvRqst) {
            serviceTaskStatus[taskStatus]++;
        } else {
            containerTaskStatus[taskStatus]++;
            containerTaskStatusPerAgent[std::make_pair(agName, taskStatus)]++;
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
            containerTaskStatusPerAgent[std::make_pair(agName, oldStatus)]--;
            containerTaskStatusPerAgent[std::make_pair(agName, taskStatus)]++;

            TaskStatusSpectra spec = convertTaskStatusToSpectra(agName);

            const std::string & hostIp = task.taskHost();
            ProcessingHostInfo * procHostInfo = Config::procFmkInfo->hostsInfo[hostIp];
            TRC("HOSTIP: " + hostIp);
            for (auto & agi : procHostInfo->agInfo) {
                TRC("-- AGNAME: " + agi.name);
                if (agi.name == agName) {
                    agi.taskStatus = spec;
                    TRC("Placing spectrum " + spec.toJsonStr() +
                        " placed for " + agName);
                }
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
    //sendProcFmkInfoUpdate();

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
    //TaskStatusSpectra spec;

    std::map<TaskStatusPerAgent, int>::iterator
        nonValid = containerTaskStatusPerAgent.end();

    auto getNumOf = [&] (TaskStatus s) -> int
        { std::map<TaskStatusPerAgent, int>::iterator
          it = containerTaskStatusPerAgent.find(std::make_pair(agName, s));
          return (it == nonValid) ? 0 : it->second; };

    TaskStatusSpectra spec(getNumOf(TASK_RUNNING),
                           getNumOf(TASK_SCHEDULED),
                           getNumOf(TASK_PAUSED),
                           getNumOf(TASK_STOPPED),
                           getNumOf(TASK_FAILED),
                           getNumOf(TASK_FINISHED));
    TRC("~~~~~> " << agName << ": "
        << spec.scheduled << ", "
        << spec.running << ", "
        << spec.paused << ", "
        << spec.stopped << ", "
        << spec.failed << ", "
        << spec.finished << " = "
        << spec.total);

    return spec;

    // return spec;
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
    std::unique_lock<std::mutex> ulck(mtxHostInfo);

    Message<MsgBodyTSK> msg(m);
    MsgBodyTSK & body = msg.body;
    Json::FastWriter fastWriter;
    std::string s(fastWriter.write(body["info"]));
    HostInfo hostInfo(s);
    std::string hostIp = hostInfo.hostIp;
    TRC("Consolidating " + s + " for host " + hostIp);
    double cpu1, cpu2;

    switch (Config::agentMode[hostIp]) {
    case CONTAINER:
        cpu1 = Config::procFmkInfo->hostsInfo[hostIp]->hostInfo.cpuInfo.overallCpuLoad.computedLoad;
        Config::procFmkInfo->hostsInfo[hostIp]->hostInfo = hostInfo;
        cpu2 = Config::procFmkInfo->hostsInfo[hostIp]->hostInfo.cpuInfo.overallCpuLoad.computedLoad;
        TRC("@@@@@@@@@@ CONSOLIDATING CONT FMK INFO @@@@@@@@@@ " << cpu1 << " => " << cpu2);
        break;
    case SERVICE:
        cpu1 = Config::procFmkInfo->swarmInfo[hostIp]->hostInfo.cpuInfo.overallCpuLoad.computedLoad;
        Config::procFmkInfo->swarmInfo[hostIp]->hostInfo = hostInfo;
        cpu2 = Config::procFmkInfo->swarmInfo[hostIp]->hostInfo.cpuInfo.overallCpuLoad.computedLoad;
        TRC("@@@@@@@@@@ CONSOLIDATING SRV FMK INFO @@@@@@@@@@ " << cpu1 << " => " << cpu2);
        break;
    default:
        break;
    }
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
    msg.buildHdr(ChnlTskRepDist, msgType, CHNLS_IF_VERSION,
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
    std::unique_lock<std::mutex> ulck(mtxHostInfo);

    // Prepare message and send it
    Message<MsgBodyTSK> msg;
    MsgBodyTSK body;

    double cpu = Config::procFmkInfo->hostsInfo.begin()->second->hostInfo.cpuInfo.overallCpuLoad.computedLoad;
    TRC("@@@@@@@@@@  => " << cpu);

    std::string s = Config::procFmkInfo->toJsonStr();
    JValue fmkInfoValue(s);
    body["info"] = fmkInfoValue.val();
    msg.buildBody(body);

    // Set message header
    msg.buildHdr(ChnlTskRepDist, MsgFmkMon, CHNLS_IF_VERSION,
                 compName, "*", "", "", "");
    // Send msg
    std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
    ChannelDescriptor chnl(ChnlTskRepDist);
    it = connections.find(chnl);
    if (it != connections.end()) {
        ScalabilityProtocolRole * conn = it->second;
        conn->setMsgOut(msg.str());
        TRC("@@@@@@@@@@ SENDING UPDATE OF FMK INFO @@@@@@@@@@");
        TRC(s);
    } else {
        ErrMsg("Couldn't send updated ProcessingFrameworkInfo data.");
    }

    // Arm new timer
    if (sendingPeriodicFmkInfo) { armProcFmkInfoMsgTimer(); }
}


//}
