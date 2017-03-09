// -*- C++ -*-

#ifndef COMMNODE_H
#define COMMNODE_H

#include "scalprotrole.h"
#include "nncommon.h"

class CommNode {
public:
    CommNode() {}
    virtual ~CommNode() {}
    virtual void addConnection(ChannelDescriptor & chnl,
                               ScalabilityProtocolRole * conct) = 0;
    virtual std::string getName() = 0;
    virtual std::string getAddress() = 0;
};

#endif
