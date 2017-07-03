// -*- C++ -*-

#ifndef SURVEY_H
#define SURVEY_H

#include "scalprotrole.h"
#include <nanomsg/survey.h>

#include "nncommon.h"

//-----------------------------------------------------------------------------
// Survey
//-----------------------------------------------------------------------------
class Survey : public ScalabilityProtocolRole {
public:
    Survey(int elemCls, std::string addr);
    Survey(int elemCls, const char * addr);
    virtual void setMsgOut(MessageString m);
    void setNumOfRespondents(int r);
protected:
    virtual void init(int elemCls, const char * addr);
    virtual void getIncommingMessageStrings();
    virtual void processMessageString(MessageString & m);
private:
    int maxRespondents;
    bool surveyorWaiting;
};

#endif
