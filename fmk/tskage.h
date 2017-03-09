// -*- C++ -*-

#ifndef TSKAGE_H
#define TSKAGE_H

#include "component.h"

class TskAge : public Component {
public:
    TskAge(const char * name, const char * addr = 0, Synchronizer * s = 0);
    TskAge(std::string name, std::string addr = std::string(), Synchronizer * s = 0);
    virtual void runEachIteration();
};
#endif
