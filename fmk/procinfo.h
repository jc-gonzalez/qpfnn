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

#ifndef PROCINFO_H
#define PROCINFO_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - vector
//------------------------------------------------------------
#include <vector>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - hostinfo.h
//------------------------------------------------------------
#include "hostinfo.h"

/*
  struct AgentInfo {
  int    idx;
  int    runningTasks;
  int    failedTasks;
  int    finishedTasks;
  int    launchedTasks;
  double load;
  };*/

struct TaskStatusSpectra {
    int    running;
    int    scheduled;
    int    paused;
    int    stopped;
    int    failed;
    int    finished;
};

struct AgentInfo {
    std::string       name;
    TaskStatusSpectra taskStatus;
    float             load;
};

struct ProcessingHostInfo {
    std::string            name;
    int                    numAgents;
    HostInfo               hostInfo;
    std::vector<AgentInfo> agInfo;
    int                    numTasks;
};

struct SwarmInfo {
    std::string       name;
    std::string       scale;
    HostInfo          hostInfo;
    TaskStatusSpectra taskStatus;
};

struct QPFInfo {
    std::vector<ProcessingHostInfo> hostsInfo;
    SwarmInfo                       swarmInfo;
    int                             numSrvTasks;
    int                             numContTasks;
};

#endif // PROCINFO_H
