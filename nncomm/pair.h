// -*- C++ -*-

#ifndef PAIR_H
#define PAIR_H

#include "scalprotrole.h"
#include <nanomsg/pair.h>

//-----------------------------------------------------------------------------
// Pair
//-----------------------------------------------------------------------------
class Pair : public ScalabilityProtocolRole {
public:
    Pair(int elemCls, std::string addr);
    Pair(int elemCls, const char * addr);
    virtual void setMsgOut(MessageString m);
protected:
    virtual void init(int elemCls, const char * addr);
    virtual void getIncommingMessageStrings();
    virtual void processMessageString(MessageString & m);
};

#endif
