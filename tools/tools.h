/******************************************************************************
 * File:    tools.h
 *          This file is part of the LibComm communications library
 *
 * Domain:  LibComm.LibComm.Tools
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
 *   Declare Tools base class
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

#ifndef TOOLS_H
#define TOOLS_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - vector
//   - sstream
//------------------------------------------------------------

#include <vector>
#include <sstream>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   none
//------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// Namespace: LibComm
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace LibComm {

//----------------------------------------------------------------------
// Function: waitForHeartBeat
// Wait for a numner of seconds and microseconds
//
// Parameters:
//   speriod  - (I) Number of seconds
//   usperiod - (I) Number of microseconds
//----------------------------------------------------------------------
void waitForHeartBeat(int speriod = 1, int usperiod = 0);

//----------------------------------------------------------------------
// Function: getYMD
// Return Year, Month and Day from a date string in the form YYYYMMDDThhmmss
//
// Parameters:
//   dateStr - (I) Original date string
//   year    - (O) Number of year
//   month   - (O) Month number
//   day     - (O) Month day number
//----------------------------------------------------------------------
void getYMD(char * dateStr, int & year, int & month, int & day);

//----------------------------------------------------------------------
// Function: timeTag
// Build a time tag string from current time
//
// Returns:
//   Tag with local date+time in the format %Y%m%dT%H%M%S
//----------------------------------------------------------------------
std::string timeTag();

//----------------------------------------------------------------------
// Function: sessionTag
// Returns a new tag for the session, or the one the user specified
//----------------------------------------------------------------------
std::string sessionTag();

//----------------------------------------------------------------------
// Function: setSessionTag
// Sets the session tag to the one specified y the user
//----------------------------------------------------------------------
void setSessionTag(std::string userSessionTag = std::string());

//----------------------------------------------------------------------
// Function: preciseTimeTag
// Build a time tag string from current time with ns precision
//
// Returns:
//   Tag with local date+time in the format %Y%m%dT%H%M%S.%ns
//----------------------------------------------------------------------
std::string preciseTimeTag();

//----------------------------------------------------------------------
// Function: tagToTimestamp
// Get date and time components from time tag
//----------------------------------------------------------------------
std::string tagToTimestamp(std::string tag);

//----------------------------------------------------------------------
// Function: waitUntilNextSecond
// Loops until next second is reached
//----------------------------------------------------------------------
void waitUntilNextSecond();

//----------------------------------------------------------------------
// Function: mkTmpFileName
// Uses mkstemp to create a temporary file according to template
//----------------------------------------------------------------------
int mkTmpFileName(char * tpl, std::string & sfName, bool closeFd = true);

//----------------------------------------------------------------------
// Function: setDebugInfo
// Sets flag for enabling debug information
//----------------------------------------------------------------------
void setDebugInfo(bool enable = true);

//----------------------------------------------------------------------
// Function: getDebugInfo
// Returns the current value of flag for enabling debug information
//----------------------------------------------------------------------
bool getDebugInfo();

struct CPUData {
   unsigned long long int totalTime;
   unsigned long long int userTime;
   unsigned long long int systemTime;
   unsigned long long int systemAllTime;
   unsigned long long int idleAllTime;
   unsigned long long int idleTime;
   unsigned long long int niceTime;
   unsigned long long int ioWaitTime;
   unsigned long long int irqTime;
   unsigned long long int softIrqTime;
   unsigned long long int stealTime;
   unsigned long long int guestTime;

   unsigned long long int totalPeriod;
   unsigned long long int userPeriod;
   unsigned long long int systemPeriod;
   unsigned long long int systemAllPeriod;
   unsigned long long int idleAllPeriod;
   unsigned long long int idlePeriod;
   unsigned long long int nicePeriod;
   unsigned long long int ioWaitPeriod;
   unsigned long long int irqPeriod;
   unsigned long long int softIrqPeriod;
   unsigned long long int stealPeriod;
   unsigned long long int guestPeriod;
};

struct MemData {
    unsigned long long int totalMem;
    unsigned long long int usedMem;
    unsigned long long int freeMem;
    unsigned long long int sharedMem;
    unsigned long long int buffersMem;
    unsigned long long int cachedMem;
    unsigned long long int totalSwap;
    unsigned long long int usedSwap;
    unsigned long long int freeSwap;
};

struct SysInfoMeter {
    double               total;
    std::vector<double>  values;
};

struct SysInfo {
    SysInfo() :
        detailedCPUinfo(false),
        accoutGuestInCPUstats(false) {}

    int                  cpuCount;
    std::vector<CPUData> cpuData;
    MemData              memData;
    std::vector<double>  loadAvgs;
    int                  upTime;

    std::vector<SysInfoMeter> cpuStat;
    std::vector<double>  cpuPercent;
    SysInfoMeter         memStat;
    SysInfoMeter         swapStat;

    bool                 detailedCPUinfo;
    bool                 accoutGuestInCPUstats;

    bool update();
    void computeStats();
};

#ifndef PROCDIR
#define PROCDIR "/proc"
#endif

#ifndef PROCSTATFILE
#define PROCSTATFILE PROCDIR "/stat"
#endif

#ifndef PROCMEMINFOFILE
#define PROCMEMINFOFILE PROCDIR "/meminfo"
#endif

#ifndef PROCUPTIME
#define PROCUPTIME PROCDIR "/uptime"
#endif

#ifndef PROCLOADAVG
#define PROCLOADAVG PROCDIR "/loadavg"
#endif

std::vector<double> getLoadAvgs();
bool scanMemoryInfo(MemData & memData);
double scanCPUTime(SysInfo * sysInfo);

//}

#endif  /* TOOLS_H */
