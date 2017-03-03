#include "tskmng.h"

#include "channels.h"

TskMng::TskMng(const char * name, const char * addr) : Component(name, addr)
{
}

TskMng::TskMng(std::string name, std::string addr) : Component(name, addr)
{
}

void TskMng::runEachIteration() {
    if (((iteration + 1) % 21) == 0) {
        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        it = connections.find(ChnlMonit);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            char msg[128] = "=====================>> Pues si...";
            conn->setMsgOut(msg);
        }
    }
    if (((iteration + 1) % 11) == 0) {
        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        it = connections.find(ChnlTskProc);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            char msg[128] = "=====================>> HOLA";
            conn->setMsgOut(msg);
        }
    }
}
