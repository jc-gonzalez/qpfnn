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
              << h.json() << "\n";
}
