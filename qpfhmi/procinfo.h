#ifndef PROCINFO_H
#define PROCINFO_H

#include "hostinfo.h"
#include <vector>

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
