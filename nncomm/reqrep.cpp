#include "reqrep.h"

//-----------------------------------------------------------------------------
// ReqRep
//-----------------------------------------------------------------------------
ReqRep::ReqRep(int elemCls, std::string addr)
{
    init(elemCls, addr.c_str());
}

ReqRep::ReqRep(int elemCls, const char * addr)
{
    init(elemCls, addr);
}

void ReqRep::init(int elemCls, const char * addr)
{
    elemClass = elemCls;
    createSocket(elemClass);
    if (elemClass == NN_REP) {
        endPoint = sck->bind(addr);
        TRC("BIND >> " << addr);
    } else {
        endPoint = sck->connect(addr);
        TRC("CONNECT >> " << addr);
    }
    address = std::string(addr);
    (void)usleep(WAIT_BINDING);
}

void ReqRep::getIncommingMessageStrings()
{
    incMsgsMask = NN_IN;
    ScalabilityProtocolRole::getIncommingMessageStrings();
}

void ReqRep::processMessageString(MessageString & m)
{
    DBG("I (" << elemName << ") got a message: '" << m << "'");
}
