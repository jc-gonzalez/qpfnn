#include "tskmng.h"

#include "channels.h"

TskMng::TskMng(const char * name, const char * addr, Synchronizer * s) : Component(name, addr, s)
{
}

TskMng::TskMng(std::string name, std::string addr, Synchronizer * s) : Component(name, addr, s)
{
}


void TskMng::runEachIteration() {
    if (((iteration + 1) % 11) == 0) {
        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        it = connections.find(ChnlTskRep);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            char msg[128] = "=====================>> HOLA";
            conn->setMsgOut(msg);
        }
    }
}
