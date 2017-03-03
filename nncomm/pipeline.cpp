#include "pipeline.h"

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------
Pipeline::Pipeline(int elemCls, std::string addr)
{
    init(elemCls, addr.c_str());
}

Pipeline::Pipeline(int elemCls, const char * addr)
{
    init(elemCls, addr);
}

void Pipeline::setMsgOut(MessageString m)
{
    if (elemClass == NN_PUSH) {
        ScalabilityProtocolRole::setMsgOut(m);
    }
}

void Pipeline::init(int elemCls, const char * addr)
{
    elemClass = elemCls;
    createSocket(elemClass);
    if (elemClass == NN_PUSH) {
        endPoint = sck->bind(addr);
    } else {
        endPoint = sck->connect(addr);
    }
    address = std::string(addr);
    (void)usleep(WAIT_BINDING);
}

void Pipeline::getIncommingMessageStrings()
{
    if (elemClass == NN_PULL) {
        incMsgsMask = NN_IN;
        ScalabilityProtocolRole::getIncommingMessageStrings();
    }
}

void Pipeline::processMessageString(MessageString & m)
{
    if (elemClass == NN_PULL) {
        DBG("I (" << elemName << ") got a message: '" << m << "'");
    }
}
