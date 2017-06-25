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
//   - map
//------------------------------------------------------------
#include <vector>
#include <map>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - hostinfo.h
//------------------------------------------------------------
#include "hostinfo.h"

struct BasicInfoContainer {
    virtual std::string toJsonStr() = 0;
    virtual void fromStr(std::string s) = 0;
};

struct TaskStatusSpectra : public BasicInfoContainer {
    TaskStatusSpectra() :
        running(0), scheduled(0), paused(0),
        stopped(0), failed(0), finished(0), total(0) {}
    TaskStatusSpectra(int r, int s, int p, int st, int fl, int f) :
        running(r), scheduled(s), paused(p),
        stopped(st), failed(fl), finished(f), total(r+s+p+st+fl+f) {}
    int    running;
    int    scheduled;
    int    paused;
    int    stopped;
    int    failed;
    int    finished;
    int    total;
    virtual std::string toJsonStr();
    virtual void fromStr(std::string s);
};

struct AgentInfo : public BasicInfoContainer {
    std::string       name;
    TaskStatusSpectra taskStatus;
    float             load;
    virtual std::string toJsonStr();
    virtual void fromStr(std::string s);
};

struct ProcessingHostInfo : public BasicInfoContainer {
    std::string            name;
    HostInfo               hostInfo;
    int                    numAgents;
    std::vector<AgentInfo> agInfo;
    int                    numTasks;
    virtual std::string toJsonStr();
    virtual void fromStr(std::string s);
};

struct SwarmInfo : public BasicInfoContainer {
    std::string       name;
    int               scale;
    HostInfo          hostInfo;
    TaskStatusSpectra taskStatus;
    virtual std::string toJsonStr();
    virtual void fromStr(std::string s);
};

struct ProcessingFrameworkInfo : public BasicInfoContainer {
    std::map<std::string,
        ProcessingHostInfo*>        hostsInfo;
    SwarmInfo                       swarmInfo;
    int                             numSrvTasks;
    int                             numContTasks;
    virtual std::string toJsonStr();
    virtual void fromStr(std::string s);
};

#endif // PROCINFO_H
