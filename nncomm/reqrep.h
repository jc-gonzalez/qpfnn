// -*- C++ -*-

#ifndef REQREP_H
#define REQREP_H

#include "scalprotrole.h"
#include <nanomsg/reqrep.h>

//-----------------------------------------------------------------------------
// ReqRep
//-----------------------------------------------------------------------------
class ReqRep : public ScalabilityProtocolRole {
public:
    ReqRep(int elemCls, std::string addr);
    ReqRep(int elemCls, const char * addr);
protected:
    virtual void init(int elemCls, const char * addr);
    virtual void getIncommingMessageStrings();
    virtual void processMessageString(MessageString & m);
};

#endif
