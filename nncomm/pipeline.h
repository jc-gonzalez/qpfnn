// -*- C++ -*-

#ifndef PIPELINE_H
#define PIPELINE_H

#include "scalprotrole.h"
#include <nanomsg/pipeline.h>

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------
class Pipeline : public ScalabilityProtocolRole {
public:
    Pipeline(int elemCls, std::string addr);
    Pipeline(int elemCls, const char * addr);
    virtual void setMsgOut(MessageString m);
protected:
    virtual void init(int elemCls, const char * addr);
    virtual void getIncommingMessageStrings();
    virtual void processMessageString(MessageString & m);
};

#endif
