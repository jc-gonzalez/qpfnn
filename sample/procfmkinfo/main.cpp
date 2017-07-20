// -*- C++ -*-

#include <iostream>
#include <unistd.h>

#include "procinfo.h"

#include <cstdlib>

int main(int argc, char * argtv[])
{
    srand(time(0));

    ProcessingFrameworkInfo q;

    int k = 0;
    int numag[] = {10,8,12};

    q.numContTasks = 0;
    q.numSrvTasks = 0;
    HostInfo hi;
    hi.update();
    hi.cpuInfo.overallCpuLoad.timeInterval = 1;
    sleep(1);

    for (auto & h : {"host1.name", "host2.name", "host3.name"}) {
        hi.update();
        ProcessingHostInfo * php = new ProcessingHostInfo;
        ProcessingHostInfo & ph = *php;
        ph.name = h;
        ph.numAgents = numag[k];
        ph.hostInfo = hi;
        ph.numTasks = 0;

        for (int i = 0; i < ph.numAgents; ++i) {
            AgentInfo agInfo;
            char buf[100];
            sprintf(buf, "TskAge_%02d_%02d", k + 1, i + 1);
            agInfo.name = std::string(buf);
            agInfo.taskStatus = TaskStatusSpectra(rand() % 10, rand() % 10, rand() % 4,
                                                  rand() % 3, rand() % 3, rand() % 10);
            agInfo.load = (rand() % 1000) * 0.01;
            ph.agInfo.push_back(agInfo);
            ph.numTasks += agInfo.taskStatus.total;
        }

        q.hostsInfo[ph.name] = php;
        q.numContTasks += ph.numTasks;
        k++;
    }


    std::cerr << q.toJsonStr() << "\n";
    std::string s = q.toJsonStr();
    q.hostsInfo["host1.name"]->numTasks = 25;
    std::cerr << q.toJsonStr() << "\n";
    q.fromStr(s);
    std::cerr << q.toJsonStr() << "\n";

    return 0;
}
