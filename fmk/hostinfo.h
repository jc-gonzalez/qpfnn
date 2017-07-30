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
#include <thread>
#include <mutex>

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
    HostInfo();
    HostInfo(std::string s);
    HostInfo(const HostInfo &obj);
    HostInfo & operator=(const HostInfo & obj);

    struct LoadAvg {
        float load1min;
        float load5min;
        float load15min;
        unsigned int runProc;
        unsigned int totalProc;
        unsigned int lastPid;
    };

    struct CPULoad {
        CPULoad() :
            workJiffies(0),
            totalJiffies(0),
            workJiffies2(0),
            totalJiffies2(0),
            timeInterval(0),
            computedLoad(0.) {}
        unsigned int workJiffies;
        unsigned int totalJiffies;
        unsigned int workJiffies2;
        unsigned int totalJiffies2;
        unsigned int timeInterval;
        float computedLoad;
    };

    struct CPUInfo {
        std::string vendor;
        std::string modelName;
        std::string architecture;
        float cpuFreq; // MHz
        unsigned int numCpus;
        unsigned int numPhysicalCpus;
        unsigned int numCoresPerSocket;
        unsigned int numThreadsPerCore;
        unsigned int cacheSize;
        bool hyperthreading;
        CPULoad overallCpuLoad;
        std::vector<CPULoad> cpuLoad;
    };

public:
    CPUInfo cpuInfo;
    LoadAvg loadAvg;
    std::string hostIp;

public:
    void update();
    std::string dump();
    std::string toJsonStr();
    void fromStr(std::string s);

private:
    void getLoadAvg(LoadAvg & l);
    void getCPULoad(CPULoad & c, int line = 0);
    void getCPUInfo(CPUInfo & info);
    void getHostInfo();

    bool firstUpdate;
    std::mutex mtxHostInfo;

    static long USER_HZ;
};

//}

#endif  /* TASKAGENT_H */
