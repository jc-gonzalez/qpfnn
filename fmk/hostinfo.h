/******************************************************************************
 * File:    hostinfo.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.HostInfo
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
 *   Declare HostInfo class
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

#ifndef HOSTINFO_H
#define HOSTINFO_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - thread
//------------------------------------------------------------
#include <vector>
#include <string>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   none
//------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: HostInfo
//==========================================================================
class HostInfo {

public:
    struct LoadAvg {
        float load1min;
        float load5min;
        float load15min;
        int   runProc;
        int   totalProc;
        int   lastPid;
    };

    struct CPULoad {
        int workJiffies;
        int totalJiffies;
        int workJiffies2;
        int totalJiffies2;
        int timeInterval;
        float computedLoad;
    };

    struct CPUInfo {
        std::string vendor;
        std::string modelName;
        std::string architecture;
        float cpuFreq; // MHz
        int numCpus;
        int numPhysicalCpus;
        int numCoresPerSocket;
        int numThreadsPerCore;
        int cacheSize;
        bool hyperthreading;
        CPULoad overallCpuLoad;
        std::vector<CPULoad> cpuLoad;
    };

public:
    CPUInfo cpuInfo;
    LoadAvg loadAvg;

public:
    void update();
    std::string dump();
    std::string toJsonStr();
    void fromStr(std::string s);

private:
    void getLoadAvg(LoadAvg & l);
    void getCPULoad(CPULoad & c, int interval, int line = 0);
    void getCPUInfo(CPUInfo & info);
    void getHostInfo();
};

//}

#endif  /* TASKAGENT_H */
