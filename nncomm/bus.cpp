#include "bus.h"

//-----------------------------------------------------------------------------
// Bus
//-----------------------------------------------------------------------------
Bus::Bus(int elemCls, std::string addr) {
    init(elemCls, addr.c_str());
}

Bus::Bus(int elemCls, const char * addr) {
    init(elemCls, addr);
}

void Bus::connectTo(std::string addr) {
    connectTo(addr.c_str());
}

void Bus::connectTo(const char * addr) {
    sck->connect(addr);
}

void Bus::init(int elemCls, const char * addr) {
    elemClass = elemCls;
    createSocket(elemClass);
    endPoint = sck->bind(addr);
    TRC("BIND+CONNECT >> " << addr);
    int to = 100;
    sck->setsockopt(NN_SOL_SOCKET, NN_RCVTIMEO, &to, sizeof(to));
    address = std::string(addr);
    (void)usleep(WAIT_BINDING);
}

void Bus::processMessageString(MessageString & m) {
    DBG("BUS MessageString: '" << m << "'");
}
