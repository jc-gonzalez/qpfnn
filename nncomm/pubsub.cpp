#include "pubsub.h"

//-----------------------------------------------------------------------------
// PubSub
//-----------------------------------------------------------------------------
PubSub::PubSub(int elemCls, std::string addr)
{
    init(elemCls, addr.c_str());
}

PubSub::PubSub(int elemCls, const char * addr)
{
    init(elemCls, addr);
}

void PubSub::setMsgOut(MessageString m)
{
    if (elemClass == NN_PUB) {
        ScalabilityProtocolRole::setMsgOut(m);
    }
}

void PubSub::init(int elemCls, const char * addr)
{
    elemClass = elemCls;
    createSocket(elemClass);
    if (elemClass == NN_PUB) {
        endPoint = sck->bind(addr);
    } else {
        sck->setsockopt(NN_SUB, NN_SUB_SUBSCRIBE, "", 0);
        endPoint = sck->connect(addr);
    }
    address = std::string(addr);
    (void)usleep(WAIT_BINDING);
}

void PubSub::getIncommingMessageStrings()
{
    if (elemClass == NN_SUB) {
        incMsgsMask = NN_IN;
        ScalabilityProtocolRole::getIncommingMessageStrings();
    }
}

void PubSub::processMessageString(MessageString & m)
{
    if (elemClass == NN_SUB) {
        DBG("I (" << elemName << ") got a message: '" << m << "'");
    }
}
