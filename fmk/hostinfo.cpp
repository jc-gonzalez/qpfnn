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

void HostInfo::getCPULoad(CPULoad & c, int interval, int line)
{
    std::string tag;
    int j1, j2, j3, j4, j5, j6, j7;

    std::ifstream inFile;
    inFile.open("/proc/stat", std::ifstream::in);
    if (inFile.good()) {
        std::string l;
        for (int i = 0; i < line; ++i) { std::getline(inFile, l); }
        inFile >> tag >> j1 >> j2 >> j3 >> j4 >> j5 >> j6 >> j7;
        inFile.close();
    }

    int wJif = c.workJiffies;
    int tJif = c.totalJiffies;
    float ival = (float)(interval);

    c.totalJiffies  = c.totalJiffies2;
    c.workJiffies   = c.workJiffies2;
    c.totalJiffies2 = j1 + j2 + j3 + j4 + j5 + j6 + j7;
    c.workJiffies2  = j1 + j2 + j3;

    if ((c.timeInterval != 0) && (c.totalJiffies2 != c.totalJiffies)) {
        float workCPU  = c.workJiffies2  - c.workJiffies;
        float totalCPU = c.totalJiffies2 - c.totalJiffies;
        c.computedLoad  = (workCPU / totalCPU) * 100.;
    } else {
        c.computedLoad = 0.;
    }

    c.timeInterval = interval;
}

void HostInfo::getCPUInfo(CPUInfo & info)
{
    int numSeconds = 1;
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
        getCPULoad(info.overallCpuLoad, numSeconds);
        for (int i = 0; i < info.numCpus; ++i) {
            info.cpuLoad.push_back(CPULoad());
            getCPULoad(info.cpuLoad[i], numSeconds);
        }
        sleep(numSeconds);
    } else {
        numSeconds = info.overallCpuLoad.timeInterval;
    }

    getCPULoad(info.overallCpuLoad, numSeconds);
    for (int i = 0; i < info.numCpus; ++i) {
        getCPULoad(info.cpuLoad[i], numSeconds);
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
