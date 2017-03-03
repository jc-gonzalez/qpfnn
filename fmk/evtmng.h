// -*- C++ -*-

#ifndef EVTMNG_H
#define EVTMNG_H

#include "component.h"

class EvtMng : public Component {
public:
    EvtMng(const char * name, const char * addr = 0);
    EvtMng(std::string name, std::string addr = std::string());
    virtual void runEachIteration();
};
#endif
