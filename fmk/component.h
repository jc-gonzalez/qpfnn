// -*- C++ -*-

#ifndef COMPONENT_H
#define COMPONENT_H

#include "commnode.h"

#include <chrono>
#include <ctime>

using std::chrono::system_clock;

class Component : public CommNode {
public:
    Component(const char * name, const char * addr = 0);
    Component(std::string name, std::string addr = std::string());

    virtual void run();

    void periodicMsgInChannel(ChannelDescriptor chnl, int period, MessageString msg);

protected:
    virtual void updateConnections();
    virtual void sendPeriodicMsgs();
    virtual void runEachIteration();
private:
    virtual void step();
protected:
    std::map<ChannelDescriptor, std::map<int, MessageString>> periodicMsgs;
    int iteration;
};

#endif
