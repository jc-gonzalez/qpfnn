#include "tskorc.h"

#include "channels.h"

TskOrc::TskOrc(const char * name, const char * addr, Synchronizer * s) : Component(name, addr, s)
{
}

TskOrc::TskOrc(std::string name, std::string addr, Synchronizer * s)  : Component(name, addr, s)
{
}

void TskOrc::runEachIteration() {
    /*
    if (((iteration + 1) % 12) == 0) {
        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        it = connections.find(ChnlTskSched);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            char msg[128];
            sprintf(msg, "New task scheduled at iter.# %d ...", iteration);
            conn->setMsgOut(msg);
        }
    }
    if (((iteration + 1) % 16) == 0) {
        std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
        it = connections.find(ChnlMonit);
        if (it != connections.end()) {
            ScalabilityProtocolRole * conn = it->second;
            char msg[128] = "=====================>> Pues no...";
            conn->setMsgOut(msg);
        }
    }
    */
}
