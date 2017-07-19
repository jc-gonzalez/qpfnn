#include "pair.h"

//-----------------------------------------------------------------------------
// Pair
//-----------------------------------------------------------------------------
Pair::Pair(int elemCls, std::string addr)
{
    init(elemCls, addr.c_str());
}

Pair::Pair(int elemCls, const char * addr)
{
    init(elemCls, addr);
}

void Pair::setMsgOut(MessageString m)
{
    if (elemClass == NN_PAIR) {
        ScalabilityProtocolRole::setMsgOut(m);
    }
}

void Pair::init(int elemCls, const char * addr)
{
    elemClass = elemCls;
    createSocket(elemClass);
    if (elemClass == NN_PAIR) {
        endPoint = sck->bind(addr);
        TRC("BIND >> " << addr);
    } else {
        elemClass = -elemCls;
        endPoint = sck->connect(addr);
        TRC("CONNECT >> " << addr);
    }
    address = std::string(addr);
    (void)usleep(WAIT_BINDING);
}

void Pair::getIncommingMessageStrings()
{
    if (elemClass == NN_PAIR) {
        incMsgsMask = NN_IN;
        ScalabilityProtocolRole::getIncommingMessageStrings();
    }
}

void Pair::processMessageString(MessageString & m)
{
    if (elemClass == NN_PAIR) {
        DBG("I (" << elemName << ") got a message: '" << m << "'");
    }
}
