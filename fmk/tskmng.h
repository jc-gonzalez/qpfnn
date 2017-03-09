// -*- C++ -*-

#ifndef TSKMNG_H
#define TSKMNG_H

#include "component.h"

class TskMng : public Component {
public:
    TskMng(const char * name, const char * addr = 0, Synchronizer * s = 0);
    TskMng(std::string name, std::string addr = std::string(), Synchronizer * s = 0);
//    virtual void runEachIteration();
};
#endif
