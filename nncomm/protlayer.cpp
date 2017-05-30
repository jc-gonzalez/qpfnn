#include "protlayer.h"

#include "pipeline.h"
#include "pubsub.h"
#include "survey.h"
#include "bus.h"

#include <sstream>
#include <string>

#define T(x)  std::string(#x)
const std::string ProtocolLayer::ProtocolLayerClassName[] = { TLIST_OF_PROTLAYERCLASSES };
#undef T

ProtocolLayer::ProtocolLayer() : protLayerClass(UNDEFINED)
{
}

ProtocolLayer::ProtocolLayerClass ProtocolLayer::getClass()
{
    return protLayerClass;
}

std::string ProtocolLayer::getClassName()
{
    return ProtocolLayerClassName[protLayerClass];
}

void ProtocolLayer::createPipeline(ChannelDescriptor chnl,
                                   CommNode* pusher,
                                   CommNode* puller,
                                   std::string pusherHost,
                                   std::string pullerHost,
                                   int port)
{
    TRC("Creating Pipeline " << chnl);
    std::string bindAddr;
    std::string connAddr;
    if (port <= 0) {
        bindAddr = "inproc://" + chnl;
        connAddr = bindAddr;
    } else {
        std::stringstream ssPort;
        ssPort << port;
        bindAddr = "tcp://" + pusherHost + ":" + ssPort.str();
        connAddr = "tcp://" + pullerHost + ";" + pusherHost + ":" + ssPort.str();
    }
    TRC("Binding: " << bindAddr << ", Connect from: " << connAddr);
    if (protLayerClass != PIPELINE) { components.clear(); }
    if (pusher != 0) {
        pusher->addConnection(chnl, new Pipeline(NN_PUSH, bindAddr));
        components.push_back(pusher);
    }
    if (puller != 0) {
        puller->addConnection(chnl, new Pipeline(NN_PULL, connAddr));
        components.push_back(puller);
    }
    protLayerClass = PIPELINE;
}

void ProtocolLayer::createPubSub(ChannelDescriptor chnl,
                                 std::vector<CommNode*> publishers,
                                 std::vector<CommNode*> subscribers,
                                 std::string publisherHost,
                                 int port)
{
    TRC("Creating PubSub " << chnl);
    std::string bindAddr;
    std::string connAddr;
    if (port <= 0) {
        bindAddr = "inproc://" + chnl;
        connAddr = bindAddr;
    } else {
        std::stringstream ssPort;
        ssPort << port;
        bindAddr = "tcp://*:" + ssPort.str();
        connAddr = "tcp://" + publisherHost + ":" + ssPort.str();
    }
    TRC("Binding: " << bindAddr << ", Connect from: " << connAddr);
    //if ((publishers.size() > 1) && (subscribers.size() == 1)) {
    //    bindAddr.swap(connAddr);
    //}
    components.clear();
    for (auto & c: publishers) {
        if (c != 0) {
            c->addConnection(chnl, new PubSub(NN_PUB, bindAddr));
            components.push_back(c);
        }
    }
    for (auto & c: subscribers) {
        if (c != 0) {
            c->addConnection(chnl, new PubSub(NN_SUB, connAddr));
            components.push_back(c);
        }
    }
    protLayerClass = PUBSUB;
}

void ProtocolLayer::createSurvey(ChannelDescriptor chnl,
                                 CommNode * surveyor,
                                 std::vector<CommNode*> respondents,
                                 std::string
                                 surveyorHost,
                                 int port)
{
    TRC("Creating Survey " << chnl);
    std::string bindAddr;
    std::string connAddr;
    if (port <= 0) {
        bindAddr = "inproc://" + chnl;
        connAddr = bindAddr;
    } else {
        std::stringstream ssPort;
        ssPort << port;
        bindAddr = "tcp://*:" + ssPort.str();
        connAddr = "tcp://" + surveyorHost + ":" + ssPort.str();
    }
    TRC("Binding: " << bindAddr << ", Connect from: " << connAddr);
    components.clear();
    if (surveyor != 0) {
        surveyor->addConnection(chnl, new Survey(NN_SURVEYOR,   bindAddr));
        components.push_back(surveyor);
    }
    for (auto & c: respondents) {
        if (c != 0) {
            c->addConnection(chnl, new Survey(NN_RESPONDENT, connAddr));
            components.push_back(c);
        }
    }
    protLayerClass = SURVEY;
}

void ProtocolLayer::createBus(ChannelDescriptor chnl,
                              std::vector<CommNode*> elements,
                              std::vector<std::string> addresses,
                              int port)
{
    TRC("Creating Bus " << chnl);
    std::string addr;
    std::vector<std::string> addressesToConnect;
    int i = 0;

    components.clear();
    for (auto & c: elements) {
        if (port <= 0) {
            std::stringstream ssNum;
            ssNum << i + 1;
            addr += "inproc://" + chnl + "_" + ssNum.str();
        } else {
            std::string host = addresses.at(i);
            std::stringstream ssPort;
            ssPort << port;
            addr = "tcp://" + host + ":" + ssPort.str();
        }
        if (c != 0) {
            TRC("Binding: " << addr << ", Connect from anywhere");
            Bus * conn = new Bus(NN_BUS, addr);
            for (auto & adr: addressesToConnect) { conn->connectTo(adr); }
            addressesToConnect.push_back(addr);
            c->addConnection(chnl, conn);
            components.push_back(c);
        }
        ++i;
    }
    protLayerClass = BUS;
}
