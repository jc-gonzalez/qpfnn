// -*- C++ -*-

#include <iostream>
#include <unistd.h>

#include "hostinfo.h"

int main(int argc, char * argv[])
{
    HostInfo h;

    h.update();
    std::cerr << h.dump() << "\n";

    sleep(3);

    h.update();

    std::cerr << h.dump() << "\n"
              << h.toJsonStr() << "\n"
              << "--------------------------------------------------\n\n";

    HostInfo hh;
    hh.fromStr(h.toJsonStr());

    std::cerr << hh.dump() << "\n"
              << hh.toJsonStr()
              << "--------------------------------------------------\n\n";

    std::cerr << "\n\nGraph of CPU%\n----------------------------------------\n";

    for (int i = 0; i < 60; ++i) {
        h.update();
        float cpu = h.cpuInfo.overallCpuLoad.computedLoad;
        printf("%3d %6.2f: %*s*\n", i, cpu, (int)(cpu), " ");
        sleep(1);
    }

    return 0;
}
