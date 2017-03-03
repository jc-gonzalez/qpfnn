#include "evtmng.h"

#include "channels.h"

EvtMng::EvtMng(const char * name, const char * addr) : Component(name, addr)
{
}

EvtMng::EvtMng(std::string name, std::string addr)  : Component(name, addr)
{
}

void EvtMng::runEachIteration()
{
    if (((iteration + 1) % 10) == 0) {
        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        it = connections.find(ChnlCmd);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            char msg[128];
            sprintf(msg, "Tell me your name for iter.# %d ...", iteration);
            conn->setMsgOut(msg);
        }
    }
    if (((iteration + 1) % 9) == 0) {
        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        it = connections.find(ChnlInData);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            char msg[128];
            sprintf(msg, "New incoming product at iter.# %d ...", iteration);
            conn->setMsgOut(msg);
        }
    }
}
