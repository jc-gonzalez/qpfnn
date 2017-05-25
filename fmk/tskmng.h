/******************************************************************************
 * File:    taskmng.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.TaskManager
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
 *   Declare TaskManager class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   Component
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

#ifndef TSKMNG_H
#define TSKMNG_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   none
//------------------------------------------------------------
#include <list>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - component.h
//   - tools.h
//   - message.h
//------------------------------------------------------------
#include "component.h"
#include "tools.h"
#include "message.h"
#include "httpserver.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: TaskManager
//==========================================================================
class TskMng : public Component {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    TskMng(const char * name, const char * addr = 0, Synchronizer * s = 0);

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    TskMng(std::string name, std::string addr = std::string(), Synchronizer * s = 0);

public:
    struct AgentInfo {
        int    idx;
        int    runningTasks;
        int    failedTasks;
        int    finishedTasks;
        int    launchedTasks;
        double load;
    };

    //----------------------------------------------------------------------
    // Method: runReachIteration
    //----------------------------------------------------------------------
    virtual void runEachIteration();

protected:
    //----------------------------------------------------------------------
    // Method: fromRunningToOperational
    //----------------------------------------------------------------------
    void fromRunningToOperational();

protected:
    //----------------------------------------------------------------------
    // Method: processIncommingMessages
    //----------------------------------------------------------------------
    virtual void processIncommingMessages();

    //----------------------------------------------------------------------
    // Method: processCmdMsg
    //----------------------------------------------------------------------
    void processCmdMsg(ScalabilityProtocolRole* c, MessageString & m);

    //----------------------------------------------------------------------
    // Method: processTskSchedMsg
    //----------------------------------------------------------------------
    void processTskSchedMsg(ScalabilityProtocolRole* c, MessageString & m);

    //----------------------------------------------------------------------
    // Method: processTskRqstMsg
    //----------------------------------------------------------------------
    void processTskRqstMsg(ScalabilityProtocolRole* c, MessageString & m);

    //----------------------------------------------------------------------
    // Method: processTskRepMsg
    //----------------------------------------------------------------------
    void processTskRepMsg(ScalabilityProtocolRole* c, MessageString & m);

private:
    //----------------------------------------------------------------------
    // Method: execTask
    // Execute the rule requested by Task Orchestrator
    //----------------------------------------------------------------------
    void execContainerTask();

    //----------------------------------------------------------------------
    // Method: selectAgent
    //----------------------------------------------------------------------
    std::string selectAgent();

    //----------------------------------------------------------------------
    // Method: sendTaskAgMsg
    //----------------------------------------------------------------------
    bool sendTaskAgMsg(MessageString & msg,
                       std::string agName);

    //----------------------------------------------------------------------
    // Method: sendTaskRes
    //----------------------------------------------------------------------
    bool sendTaskRes(MessageString & msg);

    //----------------------------------------------------------------------
    // Method: sendMonitInfo
    //----------------------------------------------------------------------
    bool sendMonitInfo(MessageString & msg);

private:
    std::vector<std::string>         agents;
    std::map<std::string, AgentInfo> agentInfo;

    std::list<TaskInfo>              serviceTasks;
    std::list<TaskInfo>              containerTasks;

    std::map<std::string, TaskStatus> taskRegistry;

    std::map<TaskStatus, int> containerTaskStatus;
    std::map<TaskStatus, int> serviceTaskStatus;

    std::map<std::pair<std::string, TaskStatus>, int> containerTaskStatusPerAgent;

    HttpServer * httpSrv;
};

//}

#endif // TSKMNG_H
