// -*- C++ -*-

#ifndef BUS_H
#define BUS_H

#include "scalprotrole.h"
#include <nanomsg/bus.h>

//-----------------------------------------------------------------------------
// Bus
//-----------------------------------------------------------------------------
class Bus : public ScalabilityProtocolRole {
public:
    Bus(int elemCls, std::string addr);
    Bus(int elemCls, const char * addr);
    void connectTo(std::string addr);
    void connectTo(const char * addr);
protected:
    virtual void init(int elemCls, const char * addr);
    virtual void processMessageString(MessageString & m);
};

#endif
