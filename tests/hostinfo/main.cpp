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
              << hh.toJsonStr() << "\n";

    return 0;
}
