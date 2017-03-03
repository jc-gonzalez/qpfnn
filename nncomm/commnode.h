// -*- C++ -*-

#ifndef COMMNODE_H
#define COMMNODE_H

#include "scalprotrole.h"
#include "nncommon.h"

#include <map>
#include <thread>
#include <mutex>

class CommNode {
public:
    CommNode(const char * name, const char * addr = 0);
    CommNode(std::string name, std::string addr = std::string());

    virtual void addConnection(ChannelDescriptor & chnl,
                               ScalabilityProtocolRole * conct);
    std::string getName();
    std::string getAddress();

    virtual void run();
    virtual void go();
    void setGo(bool b);
    bool getGo();
private:
    void init(std::string name, std::string addr);
protected:
    std::map<ChannelDescriptor, ScalabilityProtocolRole*> connections;
    std::string compName;
    std::string compAddress;
    std::thread thrId;
    bool readyToGo;
    std::mutex mtxGo;
};

#endif
