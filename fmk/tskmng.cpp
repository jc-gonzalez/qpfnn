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
    int i = 0;
    for (auto & a : cfg.agentNames) {
        agents.push_back(a);
        emptyInfo.idx           = i;
        emptyInfo.runningTasks  = 0;
        emptyInfo.failedTasks   = 0;
        emptyInfo.finishedTasks = 0;
        emptyInfo.launchedTasks = 0;
        emptyInfo.load          = 0;
        agentInfo[a] = emptyInfo;
        ++i;
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
    TRC("TskMng::processIncommingmessages()");
    for (auto & kv: connections) {
        const ChannelDescriptor & chnl = kv.first;
        ScalabilityProtocolRole * conn = kv.second;
        while (conn->next(m)) {
            Message<MsgBodyTSK> msg(m);
            std::string type(msg.header.type());
            DBG(compName << " received the message [" << m
                << "] through the channel " + chnl);
            if      (chnl == ChnlCmd)      { processCmdMsg(conn, m); }
            else if (chnl == ChnlTskSched) { processTskSchedMsg(conn, m); }
            else if (type == ChnlTskRqst)  { processTskRqstMsg(conn, m); }
            else if (type == ChnlTskRep)   { processTskRepMsg(conn, m); }
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
}

//----------------------------------------------------------------------
// Method: processCmdMsg
//----------------------------------------------------------------------
void TskMng::processCmdMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    MessageString ack = "ACK from " + compName;
    c->setMsgOut(ack);
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
    static bool first = true;
    if (first) {
        json taskInfoData;
        taskInfoData["taskName"] = std::string("task_name");
        taskInfoData["taskStatus"] = 42;
        containerTasks.push_back(taskInfoData);
        first = false;
    }

    // Define and set task object
    Message<MsgBodyTSK> msg(m);
    std::string agName(msg.header.source());
    DBG("TASK REQUEST FROM " << agName << " RECEIVED");

    // Create message
    msg.buildHdr(ChnlTskProc, ChnlTskProc, "1.0",
                 compName, agName, "", "", "");

    MsgBodyTSK body;

    // Select task to send
    bool isSrvRqst = (agName == "TskAgentSwarm");
    std::list<TaskInfo> * listOfTasks = (isSrvRqst) ? &serviceTasks : &containerTasks;

    /*
    // Put input products in the appropriate place
    URLHandler urlh;
    int i = 0;
    for (auto & m : task.inputs.products) {
        urlh.setProduct(m);
        ProductMetadata & mg = urlh.fromGateway2Processing();

        task.inputs.products.push_back(mg);
        task["inputs"][i] = mg.val();
        ++i;
    }
    */

    bool isTaskSent = false;
    std::string taskName;
    TaskStatus  taskStatus;

    if (listOfTasks->size() > 0) {
        json taskInfoData = listOfTasks->front().val();
        taskName = taskInfoData["taskName"].asString();
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

    std::string taskName = task.taskName();
    TaskStatus  taskStatus = TaskStatus(task.taskStatus());

    // Update registry and status maps if needed
    TaskStatus oldStatus = taskRegistry[taskName];
    if (oldStatus != taskStatus) {
        taskRegistry[taskName] = taskStatus;
        if (task.taskSet() == "SERVICE") {
            serviceTaskStatus[oldStatus]--;
            serviceTaskStatus[taskStatus]++;
        } else {
            containerTaskStatus[oldStatus]--;
            containerTaskStatus[taskStatus]++;
            std::pair<std::string, TaskStatus> oldPair =
                std::make_pair(taskName, oldStatus);
            std::pair<std::string, TaskStatus> tskPair =
                std::make_pair(taskName, taskStatus);
            containerTaskStatusPerAgent[oldPair]--;
            containerTaskStatusPerAgent[tskPair]++;
        }
    }
}

//----------------------------------------------------------------------
// Method: execTask
// Execute the rule requested by Task Orchestrator
//----------------------------------------------------------------------
void TskMng::execContainerTask()
{
/*
    std::string peName = msg->task.taskPath;
    InfoMsg("Proc.Elem. in rule: " + peName);

    // Select TaskAgent to use
    std::string agent = selectAgent();
    InfoMsg("Selected Agent is " + agent);

    // Send message to agent
    if (sendTaskAgMsg(msg, agent->name)) {
        agentInfo[agent].launchedTasks++;
        agentInfo[agent].runningTasks++;
    }
*/
}

inline double weightFunc(double load, double tasks) { return 100 * load + tasks; }

//----------------------------------------------------------------------
// Method: selectAgent
// Send a TaskProcessingMsg to the Task Manager, requesting the
// execution of a rule
//----------------------------------------------------------------------
std::string TskMng::selectAgent()
{
/*
    // Select agent with smaller number of running tasks
    int nRunTasks = -1;
    int agIdx;
    for (unsigned int i = 0; i < agents.size(); ++i) {
        Peer * p = agents.at(i);
        AgentInfo & agInfo = agentInfo[p];
        if ((nRunTasks < 0) || (nRunTasks > agInfo.runningTasks)) {
            nRunTasks = agInfo.runningTasks;
            agIdx = i;
        }
    }
*/

    // Select agent with lower weight (try to balance load)
    double weight = -1;
    double newW;
    int agIdx = 0;
    for (unsigned int i = 0; i < agents.size(); ++i) {
        std::string ag = agents.at(i);
        AgentInfo & agInfo = agentInfo[ag];
        newW = weightFunc(agInfo.load, agInfo.runningTasks);
        if ((weight < 0) || (weight > newW)) {
            weight = newW;
            agIdx = i;
        }
    }
    // Return agent peer
    return agents.at(agIdx);
}

//----------------------------------------------------------------------
// Method: sendTaskAgMsg
// Send a TaskProcessingMsg to the Task Manager, requesting the
// execution of a rule
//----------------------------------------------------------------------
bool TskMng::sendTaskAgMsg(MessageString & msg,
                                std::string agName)
{
    /*
    MessageData msgDataToAg(new MessageString);
    msgDataToAg.msg->setData(msg->getData());
    setForwardTo(agName, msgDataToAg.msg->header);
    PeerMessage * msgForAg = buildPeerMsg(msgDataToAg.msg->header.destination,
                                          msgDataToAg.msg->getDataString(),
                                          MSG_TASK_PROC);
    registerMsg(selfPeer()->name, *msgForAg);
    setTransmissionToPeer(agName, msgForAg);
    */
    return true;
}

//----------------------------------------------------------------------
// Method: sendTaskRes
// Send a TaskResMsg to the Event Manager
//----------------------------------------------------------------------
bool TskMng::sendTaskRes(MessageString & msg)
{
    /*
    // Send TASK_RES to all the recipients
    // TODO: Deprecate this channel for EvtMng in favour of DB
    std::vector<std::string> fwdRecip {"DataMng"}; //, "EvtMng"};
    for (std::string & recip : fwdRecip) {
        MessageData msgToRecip(new MessageString);
        msgToRecip.msg->setData(msg->getData());
        setForwardTo(recip, msgToRecip.msg->header);
        PeerMessage * msgForRecip = buildPeerMsg(msgToRecip.msg->header.destination,
                                                 msgToRecip.msg->getDataString(),
                                                 MSG_TASK_RES);
        registerMsg(selfPeer()->name, *msgForRecip);
        setTransmissionToPeer(recip, msgForRecip);
    }
    */
    return true;
}

//----------------------------------------------------------------------
// Method: sendMonitInfo
// Send a TaskResMsg to the Event Manager
//----------------------------------------------------------------------
bool TskMng::sendMonitInfo(MessageString & msg)
{
    /*
    // Send TASK_RES to all the recipients
    // TODO: Deprecate this channel for EvtMng in favour of DB
    std::array<std::string,1> fwdRecip = {"EvtMng"};
    for (std::string & recip : fwdRecip) {
        MessageData msgToRecip(new MessageString);
        msgToRecip.msg->setData(msg->getData());
        setForwardTo(recip, msgToRecip.msg->header);
        PeerMessage * msgForRecip = buildPeerMsg(msgToRecip.msg->header.destination,
                                                 msgToRecip.msg->getDataString(),
                                                 MSG_MONIT_INFO);
        //registerMsg(selfPeer()->name, *msgForRecip);
        setTransmissionToPeer(recip, msgForRecip);
    }*/
    return true;
}

//}
