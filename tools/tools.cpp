/******************************************************************************
 * File:    tools.cpp
 *          This file is part of the LibComm communications library
 *
 * Domain:  LibComm.LibComm.Tools
 *
 * Version:  1.1
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement Tools class
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
 *   See <ChangeLog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#include "tools.h"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>

#include <atomic>

#include <string>
#include <vector>
#include <map>

#include <cassert>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//namespace LibComm {

template<class T>
inline T max(T a, T b) { return (a > b) ? a : b; }

template<class T>
inline T min(T a, T b) { return (a < b) ? a : b; }

static std::string specificSessionTag = std::string();

//----------------------------------------------------------------------
// Function: waitForHeartBeat
// Wait for a numner of seconds and microseconds
//----------------------------------------------------------------------
void waitForHeartBeat(int speriod, int usperiod)
{
    // Get time (sec+usecs), compute usecs remaining for next period
    int period = speriod * 1000000 + usperiod;
    struct timeval tv;
    (void)gettimeofday(&tv, 0);
    int usecs = (tv.tv_sec % 1440) * 1000000 + tv.tv_usec;
    int usecsToWait = period - (usecs % period);
    usleep(usecsToWait);
}

//----------------------------------------------------------------------
// Function: getYMD
// Return Year, Month and Day from a date string in the form YYYYMMDDThhmmss
//----------------------------------------------------------------------
void getYMD(char * dateStr, int & year, int & month, int & day)
{
    int y = 0, m = 0, d = 0;
    sscanf(dateStr, "%04d%02d%02d", &y, &m, &d);
    year = y, month = m, day = d;
}

//----------------------------------------------------------------------
// Function: timeTag
// Build a time tag string from current time
//----------------------------------------------------------------------
std::string timeTag()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%Y%m%dT%H%M%S", timeinfo);
    return std::string(buffer);
}

//----------------------------------------------------------------------
// Function: sessionTag
// Returns a new tag for the session, or the one the user specified
//----------------------------------------------------------------------
std::string sessionTag()
{
    std::string tag(specificSessionTag);
    if (tag.empty()) { tag = timeTag(); }
    return tag;
}

//----------------------------------------------------------------------
// Function: setSessionTag
// Sets the session tag to the one specified y the user
//----------------------------------------------------------------------
void setSessionTag(std::string userSessionTag)
{
    if (specificSessionTag.empty()) {
        specificSessionTag = userSessionTag;
    }
}

//----------------------------------------------------------------------
// Function: preciseTimeTag
// Build a time tag string from current time with ns precision
//----------------------------------------------------------------------
std::string preciseTimeTag()
{
    struct timespec timesp;
    struct tm * timeinfo;
    char buffer[80];
    char ns[11];

    if (clock_gettime(CLOCK_REALTIME_COARSE, &timesp) != 0) {
        perror("clock_gettime");
        exit(1);
    }

    timeinfo = localtime(&(timesp.tv_sec));

    strftime(buffer, 80, "%Y%m%dT%H%M%S", timeinfo);
    sprintf(ns, ".%09ld", timesp.tv_nsec);
    return std::string(buffer) + std::string(ns);
}

//----------------------------------------------------------------------
// Function: tagToTimestamp
// Get date and time components from time tag
//----------------------------------------------------------------------
std::string tagToTimestamp(std::string tag)
{
    if (tag.empty()) { tag = "20010101T000000"; }
    return std::string(tag.substr(0,4) + "-" +
                       tag.substr(4,2) + "-" +
                       tag.substr(6,2) + " " +
                       tag.substr(9,2) + ":" +
                       tag.substr(11,2) + ":" +
                       tag.substr(13,2));
}

//----------------------------------------------------------------------
// Function: waitUntilNextSecond
// Loops until next second is reached
//----------------------------------------------------------------------
void waitUntilNextSecond()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if ((tv.tv_sec % 60) == 0) {
        do {
            gettimeofday(&tv, NULL);
        } while ((tv.tv_sec % 60) == 0);
    }
}

//----------------------------------------------------------------------
// Function: mkTmpFileName
// Uses mkstemp to create a temporary file according to template
//----------------------------------------------------------------------
int mkTmpFileName(char * tpl, std::string & sfName, bool closeFd)
{
    const int PATH_MAX = 200;
    char fname[PATH_MAX];
    int fd;

    strcpy(fname, tpl);
    if (((fd = mkstemp(fname)) != -1) and closeFd) {
        close(fd);
        unlink(fname);
    }

    sfName = std::string(fname);
    return fd;
}

//----------------------------------------------------------------------
// Function: getLoadAvgs
// Obtain load averages for host
//----------------------------------------------------------------------
std::vector<double> getLoadAvgs()
{
    double loadAvg[3];

    if (getloadavg(loadAvg, 3) < 0) {
        return std::vector<double>(3, 0.);
    } else {
        return std::vector<double> {loadAvg[0], loadAvg[1], loadAvg[2]};
    }
}

int getUptime()
{
   double uptime = 0;
   FILE* fd = fopen(PROCUPTIME, "r");
   if (fd) {
      int n = fscanf(fd, "%64lf", &uptime);
      fclose(fd);
      if (n <= 0) return 0;
   }
   return (int) floor(uptime);
}

bool scanMemoryInfo(MemData & memData)
{
    unsigned long long int swapFree = 0;

    FILE* file = fopen(PROCMEMINFOFILE, "r");
    if (file == NULL) { return false; }

    char buffer[128];
    while (fgets(buffer, 128, file)) {
        std::string b(buffer);
        switch (buffer[0]) {
        case 'M':
            if (b.compare(0,9,"MemTotal:") == 0)
                sscanf(buffer, "MemTotal: %32llu kB", &(memData.totalMem));
            else if (b.compare(0,8,"MemFree:") == 0)
                sscanf(buffer, "MemFree: %32llu kB", &(memData.freeMem));
            else if (b.compare(0,10,"MemShared:") == 0)
                sscanf(buffer, "MemShared: %32llu kB", &(memData.sharedMem));
            break;
        case 'B':
            if (b.compare(0,8,"Buffers:") == 0)
                sscanf(buffer, "Buffers: %32llu kB", &(memData.buffersMem));
            break;
        case 'C':
            if (b.compare(0,7,"Cached:") == 0)
                sscanf(buffer, "Cached: %32llu kB", &(memData.cachedMem));
            break;
        case 'S':
            if (b.compare(0,10,"SwapTotal:") == 0)
                sscanf(buffer, "SwapTotal: %32llu kB", &(memData.totalSwap));
            if (b.compare(0,9,"SwapFree:") == 0)
                sscanf(buffer, "SwapFree: %32llu kB", &swapFree);
            break;
        }
    }
    fclose(file);

    memData.usedMem  = memData.totalMem  - memData.freeMem;
    memData.usedSwap = memData.totalSwap - swapFree;

    return true;
}

double scanCPUTime(SysInfo * sysInfo)
{
    FILE* file = fopen(PROCSTATFILE, "r");
    if (file == NULL) { return -1.; }

    int cpus = sysInfo->cpuCount;
    assert(cpus > 0);
    for (int i = 0; i <= cpus; i++) {
        char buffer[256];
        unsigned long long int usertime, nicetime, systemtime, idletime;
        unsigned long long int ioWait, irq, softIrq, steal, guest, guestnice;
        ioWait = irq = softIrq = steal = guest = guestnice = 0;
        // Depending on your kernel version,
        // 5, 7, 8 or 9 of these fields will be set.
        // The rest will remain at zero.
        char* ok = fgets(buffer, 255, file);
        if (!ok) buffer[0] = '\0';
        if (i == 0) {
            sscanf(buffer,
                   "cpu  %16llu %16llu %16llu %16llu %16llu "
                   "%16llu %16llu %16llu %16llu %16llu",
                           &usertime, &nicetime, &systemtime, &idletime,
                   &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
        } else {
            int cpuid;
            sscanf(buffer,
                   "cpu%4d %16llu %16llu %16llu %16llu %16llu "
                   "%16llu %16llu %16llu %16llu %16llu",
                   &cpuid, &usertime, &nicetime, &systemtime, &idletime,
                   &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
            assert(cpuid == i - 1);
        }
        // Guest time is already accounted in usertime
        usertime = usertime - guest;
        nicetime = nicetime - guestnice;
        // Fields existing on kernels >= 2.6
        // (and RHEL's patched kernel 2.4...)
        unsigned long long int idlealltime = idletime + ioWait;
        unsigned long long int systemalltime = systemtime + irq + softIrq;
        unsigned long long int virtalltime = guest + guestnice;
        unsigned long long int totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;
        CPUData & cpuData = sysInfo->cpuData[i];
//        assert (usertime >= cpuData->userTime);
//        assert (nicetime >= cpuData->niceTime);
//        assert (systemtime >= cpuData->systemTime);
//        assert (idletime >= cpuData->idleTime);
//        assert (totaltime >= cpuData->totalTime);
//        assert (systemalltime >= cpuData->systemAllTime);
//        assert (idlealltime >= cpuData->idleAllTime);
//        assert (ioWait >= cpuData->ioWaitTime);
//        assert (irq >= cpuData->irqTime);
//        assert (softIrq >= cpuData.softIrqTime);
//        assert (steal >= cpuData.stealTime);
//        assert (virtalltime >= cpuData.guestTime);
        cpuData.userPeriod = usertime - cpuData.userTime;
        cpuData.nicePeriod = nicetime - cpuData.niceTime;
        cpuData.systemPeriod = systemtime - cpuData.systemTime;
        cpuData.systemAllPeriod = systemalltime - cpuData.systemAllTime;
        cpuData.idleAllPeriod = idlealltime - cpuData.idleAllTime;
        cpuData.idlePeriod = idletime - cpuData.idleTime;
        cpuData.ioWaitPeriod = ioWait - cpuData.ioWaitTime;
        cpuData.irqPeriod = irq - cpuData.irqTime;
        cpuData.softIrqPeriod = softIrq - cpuData.softIrqTime;
        cpuData.stealPeriod = steal - cpuData.stealTime;
        cpuData.guestPeriod = virtalltime - cpuData.guestTime;
        cpuData.totalPeriod = totaltime - cpuData.totalTime;
        cpuData.userTime = usertime;
        cpuData.niceTime = nicetime;
        cpuData.systemTime = systemtime;
        cpuData.systemAllTime = systemalltime;
        cpuData.idleAllTime = idlealltime;
        cpuData.idleTime = idletime;
        cpuData.ioWaitTime = ioWait;
        cpuData.irqTime = irq;
        cpuData.softIrqTime = softIrq;
        cpuData.stealTime = steal;
        cpuData.guestTime = virtalltime;
        cpuData.totalTime = totaltime;
    }
    fclose(file);

    CPUData & allCpuData = sysInfo->cpuData[0];
    double period = (double)(allCpuData.totalPeriod) / cpus;
    return period;
}

bool SysInfo::update()
{
    static bool first = true;

    // Get Memory info
    if (!scanMemoryInfo(this->memData)) { return false; }

    // Get load aveerages
    this->loadAvgs = getLoadAvgs();

    // Get up time
    this->upTime = getUptime();

    if (first) {
        first = false;
        // Get number of CPUs

        FILE* file = fopen(PROCSTATFILE, "r");
        if (file == NULL) { return false; }
        char buffer[256];
        int cpus = -1;
        std::string ss;
        do {
            cpus++;
            char * s = fgets(buffer, 255, file);
            (void)s;
            ss = std::string(buffer);
        } while (ss.compare(0,3,std::string("cpu")) == 0);
        fclose(file);

        this->cpuCount = max<int>(cpus - 1, 1);

        this->cpuData.resize(cpus + 1);
        this->cpuStat.resize(cpus + 1);
        this->cpuPercent.resize(cpus + 1);

        for (int i = 0; i < cpus; i++) {
            this->cpuData[i].totalTime   = 1;
            this->cpuData[i].totalPeriod = 1;
        }
    }

    // Get single CPU information
    if (scanCPUTime(this) < 0.) { return false; }

    return true;
}

#define SYSINFOMETER_MAXNUMITEMS 10

void SysInfo::computeStats()
{
    // Compute CPU stats
    for (int i = 0; i <= this->cpuCount; ++i) {
        SysInfoMeter & m = this->cpuStat[i];

        double & total = m.total;

        std::vector<double> & v = m.values;
        v.resize(SYSINFOMETER_MAXNUMITEMS);
        int numItems = 2;

        double & percent = this->cpuPercent[i];

        CPUData & cpud = this->cpuData[i];
        total = (double)(cpud.totalPeriod == 0 ? 1 : cpud.totalPeriod);
        v[0] = cpud.nicePeriod / total * 100.0;
        v[1] = cpud.userPeriod / total * 100.0;
        if (detailedCPUinfo) {
           v[2] = cpud.systemPeriod / total * 100.0;
           v[3] = cpud.irqPeriod / total * 100.0;
           v[4] = cpud.softIrqPeriod / total * 100.0;
           v[5] = cpud.stealPeriod / total * 100.0;
           v[6] = cpud.guestPeriod / total * 100.0;
           v[7] = cpud.ioWaitPeriod / total * 100.0;
           numItems = 8;
           if (accoutGuestInCPUstats) {
              percent = v[0]+v[1]+v[2]+v[3]+v[4]+v[5]+v[6];
           } else {
              percent = v[0]+v[1]+v[2]+v[3]+v[4];
           }
        } else {
           v[2] = cpud.systemAllPeriod / total * 100.0;
           v[3] = (cpud.stealPeriod + cpud.guestPeriod) / total * 100.0;
           numItems = 4;
           percent = v[0]+v[1]+v[2]+v[3];
        }
        v.resize(numItems);
        percent = min<double>(100.0, max<double>(0.0, percent));
        if (std::isnan(percent)) percent = 0.0;
    }

    // Compute Memory stats
    long int usedMem    = this->memData.usedMem;
    long int buffersMem = this->memData.buffersMem;
    long int cachedMem  = this->memData.cachedMem;
    usedMem -= buffersMem + cachedMem;
    this->memStat.total = this->memData.totalMem;
    this->memStat.values.clear();
    this->memStat.values.push_back(usedMem);
    this->memStat.values.push_back(buffersMem);
    this->memStat.values.push_back(cachedMem);

    // Compute Swap stats
    this->swapStat.total = this->memData.totalSwap;
    this->swapStat.values.clear();
    this->swapStat.values.push_back(this->memData.usedSwap);
}

//}
