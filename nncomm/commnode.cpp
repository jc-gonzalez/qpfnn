#include "commnode.h"

CommNode::CommNode(const char * name, const char * addr)
{
    init(std::string(name), std::string(addr));
}

CommNode::CommNode(std::string name, std::string addr)
{
    init(name, addr);
}

void CommNode::init(std::string name, std::string addr)
{
    compName    = name;
    compAddress = addr;
    readyToGo   = false;

    thrId = std::thread(&CommNode::run, this);
    thrId.detach();
}

std::string CommNode::getName()
{
    return compName;
}

std::string CommNode::getAddress()
{
    return compAddress;
}

void CommNode::addConnection(ChannelDescriptor & chnl,
                              ScalabilityProtocolRole * conct)
{
    conct->setName(compName);
    connections[chnl] = conct;
}

void CommNode::run()
{
    setGo(false);
    while (!getGo()) {}
}

void CommNode::go()
{
    for (auto & kv: connections) {
        ChannelDescriptor chnl = kv.first;
        ScalabilityProtocolRole * role = kv.second;
        TRC(role->getName() << "(" << this << ") in Channel " << chnl
            << " with address " << role->getAddress()
            << " - " << role->getClass());
    }
    setGo(true);
}

void CommNode::setGo(bool b)
{
    mtxGo.lock();
    readyToGo = b;
    mtxGo.unlock();
}

bool CommNode::getGo()
{
    mtxGo.lock();
    bool b = readyToGo;
    mtxGo.unlock();
    return b;
}
