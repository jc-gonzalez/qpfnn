// -*- C++ -*-

#ifndef TSKORC_H
#define TSKORC_H

#include "component.h"

class TskOrc : public Component {
public:
    TskOrc(const char * name, const char * addr = 0, Synchronizer * s = 0);
    TskOrc(std::string name, std::string addr = std::string(), Synchronizer * s = 0);
    virtual void runEachIteration();
};
#endif
