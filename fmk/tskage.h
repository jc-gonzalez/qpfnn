/******************************************************************************
 * File:    tskage.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.TskAge
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
 *   Declare Tskage class
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

#ifndef TSKAGE_H
#define TSKAGE_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - thread
//------------------------------------------------------------
#include <thread>
#include <mutex>
#include <sys/stat.h>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - component.h
//------------------------------------------------------------
#include "component.h"
#include "dckmng.h"
#include "urlhdl.h"
#include "hostinfo.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: TskAge
//==========================================================================
class TskAge : public Component {

public:
    // Running mode of the agent.  If SERVICE, a Docker Swarm is created
    enum AgentMode { CONTAINER, SERVICE };

    // Information on the service to be created, in Docker Swarm
    struct ServiceInfo {
        std::string              service;
        std::string              serviceImg;
        std::string              exe;
        std::vector<std::string> args;
        int                      scale;
    };

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    TskAge(const char * name, const char * addr = 0, Synchronizer * s = 0,
           AgentMode mode = TskAge::CONTAINER,
           const std::vector<std::string> & nds = std::vector<std::string>(),
           ServiceInfo * srvInfo = 0);

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    TskAge(std::string name, std::string addr = std::string(), Synchronizer * s = 0,
           AgentMode mode = TskAge::CONTAINER,
           const std::vector<std::string> & nds = std::vector<std::string>(),
           ServiceInfo * srvInfo = 0);

protected:

#undef T
#define TLIST_PSTATUS T(IDLE), T(WAITING), T(PROCESSING), T(FINISHING)

#define T(x) x
    enum ProcStatus { TLIST_PSTATUS };
#undef T
    static const std::string ProcStatusName[];

    //----------------------------------------------------------------------
    // Method: fromRunningToOperational
    //----------------------------------------------------------------------
    virtual void fromRunningToOperational();

    //----------------------------------------------------------------------
    // Method: runEachIteration
    //----------------------------------------------------------------------
    virtual void runEachIteration();

    //----------------------------------------------------------------------
    // Method: processTskProcMsg
    //----------------------------------------------------------------------
    virtual void processTskProcMsg(ScalabilityProtocolRole* c, MessageString & m);

private:
    //----------------------------------------------------------------------
    // Method: runEachIterationForContainers
    //----------------------------------------------------------------------
    void runEachIterationForContainers();

    //----------------------------------------------------------------------
    // Method: runEachIterationForServices
    //----------------------------------------------------------------------
    void runEachIterationForServices();

    //----------------------------------------------------------------------
    // Method: sendTaskReport
    //----------------------------------------------------------------------
    void sendTaskReport();

    //----------------------------------------------------------------------
    // Method: retrieveOutputProducts
    //----------------------------------------------------------------------
    void retrieveOutputProducts();

    //----------------------------------------------------------------------
    // Method: sendHostInfoUpdate
    //----------------------------------------------------------------------
    void sendHostInfoUpdate();

    Property(TskAge, std::string, workDir, WorkDir);
    Property(TskAge, std::string, sysDir,  SysDir);
    Property(TskAge, bool,        remote,  Remote);

private:
    AgentMode                agentMode;
    ProcStatus               pStatus;
    DockerMng *              dckMng;

    ServiceInfo *            serviceInfo;
    std::string              srvManager;
    std::vector<std::string> nodes;

    std::string              containerId;

    TaskInfo *               runningTask;
    TaskStatus               taskStatus;

    std::string              internalTaskNameIdx;
    std::string              exchangeDir;
    std::string              exchgIn;
    std::string              exchgOut;
    std::string              exchgLog;

    MessageString            origMsgString;

    int                      numTask;
    int                      waitingCycles;
    int                      maxWaitingCycles;
    int                      idleCycles;
    int                      idleCyclesBeforeRequest;
    int                      workingDuring;

    URLHandler               urlh;

    HostInfo                 hostInfo;
};

//}

#endif  /* TASKAGENT_H */
