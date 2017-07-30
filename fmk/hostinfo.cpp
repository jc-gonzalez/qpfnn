/******************************************************************************
 * File:    hostinfo.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.hostinfo
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
 *   Implement HostInfo class
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

#include "hostinfo.h"

#include "datatypes.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <unistd.h>

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

// -*- C++ -*-

long HostInfo::USER_HZ = sysconf(_SC_CLK_TCK);

HostInfo::HostInfo() : firstUpdate(true)
{
    cpuInfo.overallCpuLoad.timeInterval = 0;
}

HostInfo::HostInfo(const HostInfo &obj)
{
    hostIp = obj.hostIp;

    loadAvg.load1min   = obj.loadAvg.load1min;
    loadAvg.load5min   = obj.loadAvg.load5min;
    loadAvg.load15min  = obj.loadAvg.load15min;
    loadAvg.runProc    = obj.loadAvg.runProc;
    loadAvg.totalProc  = obj.loadAvg.totalProc;
    loadAvg.lastPid    = obj.loadAvg.lastPid;

    cpuInfo.vendor            = obj.cpuInfo.vendor;
    cpuInfo.modelName         = obj.cpuInfo.modelName;
    cpuInfo.architecture      = obj.cpuInfo.architecture;
    cpuInfo.cpuFreq           = obj.cpuInfo.cpuFreq;
    cpuInfo.numCpus           = obj.cpuInfo.numCpus;
    cpuInfo.numPhysicalCpus   = obj.cpuInfo.numPhysicalCpus;
    cpuInfo.numCoresPerSocket = obj.cpuInfo.numCoresPerSocket;
    cpuInfo.numThreadsPerCore = obj.cpuInfo.numThreadsPerCore;
    cpuInfo.cacheSize         = obj.cpuInfo.cacheSize;
    cpuInfo.hyperthreading    = obj.cpuInfo.hyperthreading;

    cpuInfo.overallCpuLoad.workJiffies   = obj.cpuInfo.overallCpuLoad.workJiffies;
    cpuInfo.overallCpuLoad.totalJiffies  = obj.cpuInfo.overallCpuLoad.totalJiffies;
    cpuInfo.overallCpuLoad.workJiffies2  = obj.cpuInfo.overallCpuLoad.workJiffies2;
    cpuInfo.overallCpuLoad.totalJiffies2 = obj.cpuInfo.overallCpuLoad.totalJiffies2;
    cpuInfo.overallCpuLoad.timeInterval  = obj.cpuInfo.overallCpuLoad.timeInterval;
    cpuInfo.overallCpuLoad.computedLoad  = obj.cpuInfo.overallCpuLoad.computedLoad;

    cpuInfo.cpuLoad.clear();
    for (int i = 0; i < cpuInfo.numCpus; ++i) {
        CPULoad c;
        c.workJiffies   = obj.cpuInfo.cpuLoad[i].workJiffies;
        c.totalJiffies  = obj.cpuInfo.cpuLoad[i].totalJiffies;
        c.workJiffies2  = obj.cpuInfo.cpuLoad[i].workJiffies2;
        c.totalJiffies2 = obj.cpuInfo.cpuLoad[i].totalJiffies2;
        c.timeInterval  = obj.cpuInfo.cpuLoad[i].timeInterval;
        c.computedLoad  = obj.cpuInfo.cpuLoad[i].computedLoad;
        cpuInfo.cpuLoad.push_back(c);
    }
}


void HostInfo::update()
{
    getHostInfo();
}

std::string HostInfo::dump()
{
    LoadAvg & l = loadAvg;
    CPUInfo & c = cpuInfo;
    std::stringstream s;
    s << "Host: " << hostIp << "\n"
      << "Hardware: " << c.modelName
      << " " << c.architecture
      << " (" << c.vendor << ")\n"
      << "CPU Freq. / L2 Cache: " << c.cpuFreq << "MHz / "
      << c.cacheSize << "KB\n"
      << "Number of CPUs: " << c.numCpus << " : "
      << c.numPhysicalCpus << " physical sockets with "
      << c.numCoresPerSocket << " cores per socket and "
      << c.numThreadsPerCore << " threads per core "
      << "[hyperthreading:" << (c.hyperthreading ? "ON" : "OFF") << "]\n"
      << "Overall CPU Load: " << c.overallCpuLoad.computedLoad
      << "% over " << c.overallCpuLoad.timeInterval << "s\n"
      << "Load averages: "
      << l.load1min << " / "
      << l.load5min << " / "
      << l.load15min << "\n"
      << "Processes: " << l.runProc << " running of a total of "
      << l.totalProc << "\n"
      << "Last PID: " << l.lastPid << "\n";
    return s.str();
}

std::string HostInfo::toJsonStr()
{
    LoadAvg & l = loadAvg;
    CPUInfo & c = cpuInfo;
    std::stringstream s;
    s << "{\"ip\":\"" << hostIp << "\",";

    s << "\"loadAvg\":{"
      << "\"load1min\":" << l.load1min << ","
      << "\"load5min\":" << l.load5min << ","
      << "\"load15min\":" << l.load15min << ","
      << "\"runProc\":" << l.runProc << ","
      << "\"totalProc\":" << l.totalProc << ","
      << "\"lastPid\":" << l.lastPid << "},";

    s << "\"cpuInfo\":{"
      << "\"vendor\":\"" << c.vendor << "\","
      << "\"modelName\":\"" << c.modelName << "\","
      << "\"architecture\":\"" << c.architecture << "\","
      << "\"cpuFreq\":" << c.cpuFreq << "," // MHz
      << "\"numCpus\":" << c.numCpus << ","
      << "\"numPhysicalCpus\":" << c.numPhysicalCpus << ","
      << "\"numCoresPerSocket\":" << c.numCoresPerSocket << ","
      << "\"numThreadsPerCore\":" << c.numThreadsPerCore << ","
      << "\"cacheSize\":" << c.cacheSize << "," // KB
      << "\"hyperthreading\":" << c.hyperthreading << ",";

    CPULoad & o = c.overallCpuLoad;
    s << "\"overallCpuLoad\":{"
      << "\"jiffies\":[" << o.workJiffies << ","
      << o.totalJiffies << ","
      << o.workJiffies2 << ","
      << o.totalJiffies2 << "],"
      << "\"timeInterval\":" << o.timeInterval << "," // s
      << "\"computedLoad\":" << o.computedLoad << "},"
      << "\"cpuLoad\":[";

    for (int i = 0; i < c.numCpus; ++i) {
        CPULoad & o = c.cpuLoad[i];
        s << "{"
          << "\"jiffies\":[" << o.workJiffies << ","
          << o.totalJiffies << ","
          << o.workJiffies2 << ","
          << o.totalJiffies2 << "],"
          << "\"timeInterval\":" << o.timeInterval << "," // s
          << "\"computedLoad\":" << o.computedLoad << "}";
        if (i < (c.numCpus - 1)) { s << ","; }
    }
    s << "]}}";

    return s.str();
}

void HostInfo::fromStr(std::string s)
{
    LoadAvg & l = loadAvg;
    CPUInfo & c = cpuInfo;

    JValue h(s);

    hostIp = h["ip"].asString();

    JValue hl(h["loadAvg"]);
    l.load1min  = hl["load1min"].asFloat();
    l.load5min  = hl["load5min"].asFloat();
    l.load15min = hl["load15min"].asFloat();
    l.runProc   = hl["runProc"].asInt();
    l.totalProc = hl["totalProc"].asInt();
    l.lastPid   = hl["lastPid"].asInt();

    JValue hc(h["cpuInfo"]);
    c.vendor            = hc["vendor"].asString();
    c.modelName         = hc["modelName"].asString();
    c.architecture      = hc["architecture"].asString();
    c.cpuFreq           = hc["cpuFreq"].asFloat(); // MHz
    c.numCpus           = hc["numCpus"].asInt();
    c.numPhysicalCpus   = hc["numPhysicalCpus"].asInt();
    c.numCoresPerSocket = hc["numCoresPerSocket"].asInt();
    c.numThreadsPerCore = hc["numThreadsPerCore"].asInt();
    c.cacheSize         = hc["cacheSize"].asInt();
    c.hyperthreading    = hc["hyperthreading"].asBool();;

    CPULoad & co = c.overallCpuLoad;
    JValue hco(hc["overallCpuLoad"]);
    co.workJiffies   = hco["jiffies"][0].asInt();
    co.totalJiffies  = hco["jiffies"][1].asInt();
    co.workJiffies2  = hco["jiffies"][2].asInt();
    co.totalJiffies2 = hco["jiffies"][3].asInt();
    co.timeInterval  = hco["timeInterval"].asInt();
    co.computedLoad  = hco["computedLoad"].asFloat();

    for (int i = 0; i < c.numCpus; ++i) {
        c.cpuLoad.push_back(CPULoad());
        CPULoad & co = c.cpuLoad[i];
        JValue hco(hc["cpuLoad"][i]);
        co.workJiffies   = hco["jiffies"][0].asInt();
        co.totalJiffies  = hco["jiffies"][1].asInt();
        co.workJiffies2  = hco["jiffies"][2].asInt();
        co.totalJiffies2 = hco["jiffies"][3].asInt();
        co.timeInterval  = hco["timeInterval"].asInt();
        co.computedLoad  = hco["computedLoad"].asFloat();
    }
}

void HostInfo::getLoadAvg(LoadAvg & l)
{
    char sep;

    std::ifstream inFile;
    inFile.open("/proc/loadavg", std::ifstream::in);
    if (inFile.good()) {
        inFile >> l.load1min >> l.load5min >> l.load15min
               >> l.runProc >> sep >> l.totalProc >> l.lastPid;
        inFile.close();
    }
}

void HostInfo::getCPULoad(CPULoad & c, int line)
{
    std::string tag;
    //int j1, j2, j3, j4, j5, j6, j7;
    int _user, _nice, _system, _idle, _iowait, _irq, _softirq, _steal, _guest, _guest_nice;

    std::ifstream inFile;
    inFile.open("/proc/stat", std::ifstream::in);
    if (inFile.good()) {
        std::string l;
        for (int i = 0; i < line; ++i) { std::getline(inFile, l); }
        inFile >> tag
               >> _user >> _nice >> _system >> _idle >> _iowait >> _irq
               >> _softirq >> _steal >> _guest >> _guest_nice;
        inFile.close();
    }

    /*------------------------------------------------------------
    // Guest time is already accounted in usertime
    usertime = usertime - guest;                     # As you see here, it subtracts guest from user time
    nicetime = nicetime - guestnice;                 # and guest_nice from nice time
    // Fields existing on kernels >= 2.6
    // (and RHEL's patched kernel 2.4...)
    idlealltime = idletime + ioWait;                 # ioWait is added in the idleTime
    systemalltime = systemtime + irq + softIrq;
    virtalltime = guest + guestnice;
    totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;
    And so, from fields listed in the first line of /proc/stat: (see section 1.8 at documentation)

         user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
    cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0
    Algorithmically, we can calculate the CPU usage percentage like:

    PrevIdle = previdle + previowait
    Idle = idle + iowait

    PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
    NonIdle = user + nice + system + irq + softirq + steal

    PrevTotal = PrevIdle + PrevNonIdle
    Total = Idle + NonIdle

    # differentiate: actual value minus the previous one
    totald = Total - PrevTotal
    idled = Idle - PrevIdle

    CPU_Percentage = (totald - idled)/totald
    ------------------------------------------------------------*/

    c.totalJiffies  = c.totalJiffies2;
    c.workJiffies   = c.workJiffies2;

    c.totalJiffies2 = (_user + _nice + _system + _idle + _iowait + _irq +
                       _softirq + _steal + _guest + _guest_nice);
    c.workJiffies2  = c.totalJiffies2 - (_idle + _iowait); // total - idle

    if (((!firstUpdate) && (c.totalJiffies2 - c.totalJiffies))) {
        float workCPU  = c.workJiffies2  - c.workJiffies;
        float totalCPU = c.totalJiffies2 - c.totalJiffies;

        c.computedLoad = (workCPU * 100.) / totalCPU;
        c.timeInterval = (int)(totalCPU * 1000) / HostInfo::USER_HZ;
        std::cerr << std::string("!!!! >>") << c.computedLoad << "\n";
    } else {
        c.computedLoad = 0.;
        c.timeInterval = 1;
        firstUpdate = false;
    }
}

void HostInfo::getCPUInfo(CPUInfo & info)
{
    int posOfColon = -1;

    std::ifstream inFile;
    inFile.open("/proc/cpuinfo", std::ifstream::in);
    std::string l, tag;
    int siblings, cores;
    info.numPhysicalCpus = 0;
    while (inFile.good()) {
        std::getline(inFile, l);
        tag = l.substr(0, l.find_first_of("\t"));
        posOfColon = l.find_first_of(":");
        if        (tag == "vendor_id") {
            info.vendor = l.substr(posOfColon + 2);
        } else if (tag == "model name") {
            info.modelName = l.substr(posOfColon + 2);
        } else if (tag == "cpu MHz") {
            info.cpuFreq = std::stof(l.substr(posOfColon + 2));
        } else if (tag == "cache size") {
            info.cacheSize = std::stol(l.substr(posOfColon + 2));
        } else if (tag == "physical id") {
            info.numPhysicalCpus = std::stol(l.substr(posOfColon + 2)) + 1;
        } else if (tag == "siblings") {
            siblings = std::stol(l.substr(posOfColon + 2));
        } else if (tag == "cpu cores") {
            cores = std::stol(l.substr(posOfColon + 2));
        } else if (tag == "flags") {
            info.architecture = ((l.find(" lm ") != std::string::npos) ?
                                 "x86_64" : "i386");
        } else {
            // go on
        }
    }
    inFile.close();

    info.numThreadsPerCore = siblings / cores;
    info.hyperthreading = siblings > cores;
    info.numCoresPerSocket = cores;
    info.numCpus = info.numPhysicalCpus * siblings;

    // Get initial values in case it is the first time
    if (info.overallCpuLoad.timeInterval < 1) {
        getCPULoad(info.overallCpuLoad);
        for (int i = 0; i < info.numCpus; ++i) {
            info.cpuLoad.push_back(CPULoad());
            getCPULoad(info.cpuLoad[i]);
        }
    }

    getCPULoad(info.overallCpuLoad);
    for (int i = 0; i < info.numCpus; ++i) {
        getCPULoad(info.cpuLoad[i]);
    }
}

//----------------------------------------------------------------------
// Method: retrieveOutputProducts
//----------------------------------------------------------------------
void HostInfo::getHostInfo()
{
    getCPUInfo(cpuInfo);
    getLoadAvg(loadAvg);
}

//}
