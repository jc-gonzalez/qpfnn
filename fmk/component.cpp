#include "component.h"

Component::Component(const char * name, const char * addr)
    : CommNode(name, addr)
{
    iteration = 0;
}

Component::Component(std::string name, std::string addr)
    : CommNode(name, addr)
{
    iteration = 0;
}

void Component::periodicMsgInChannel(ChannelDescriptor chnl, int period, MessageString msg)
{
    periodicMsgs[chnl][period] = msg;
}

void Component::updateConnections()
{
    for (auto & kv: connections) { kv.second->update(); }
}

void Component::sendPeriodicMsgs()
{
    for (auto & kv: periodicMsgs) {
        ChannelDescriptor chnl = kv.first;
        for (auto & kkv: kv.second) {
            int period = kkv.first;
            if (((iteration + 1) % period) == 0) {
                std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
                it = connections.find(chnl);
                if (it != connections.end()) {
                    ScalabilityProtocolRole * conn = it->second;
                    MessageString & msg = kkv.second;
                    conn->setMsgOut(msg);
                }
            }
        }
    }
}

void Component::runEachIteration()
{
}

void Component::step()
{
    /*
    // Sleep until next second
    std::time_t tt = system_clock::to_time_t(system_clock::now());
    struct std::tm * ptm = std::localtime(&tt);
    ptm->tm_sec++;
    std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm))); */
    // Sleep for 200 milliseconds
    //auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void Component::run()
{
    setGo(false);
    while (!getGo()) {}
    for (;;++iteration) {
        updateConnections();
        sendPeriodicMsgs();
        runEachIteration();
        step();
    }
}
