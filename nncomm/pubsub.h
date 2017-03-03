// -*- C++ -*-

#ifndef PUBSUB_H
#define PUBSUB_H

#include "scalprotrole.h"
#include <nanomsg/pubsub.h>

//-----------------------------------------------------------------------------
// PubSub
//-----------------------------------------------------------------------------
class PubSub : public ScalabilityProtocolRole {
public:
    PubSub(int elemCls, std::string addr);
    PubSub(int elemCls, const char * addr);
    virtual void setMsgOut(MessageString m);
protected:
    virtual void init(int elemCls, const char * addr);
    virtual void getIncommingMessageStrings();
    virtual void processMessageString(MessageString & m);
};

#endif
