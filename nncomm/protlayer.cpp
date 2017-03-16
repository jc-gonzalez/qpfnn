#include "protlayer.h"

#include "pipeline.h"
#include "pubsub.h"
#include "survey.h"
#include "bus.h"

#include <sstream>
#include <string>

ProtocolLayer::ProtocolLayer() : protLayerClass(UNDEFINED)
{
}

ProtocolLayer::ProtocolLayerClass ProtocolLayer::getClass()
{
    return protLayerClass;
}

void ProtocolLayer::createPipeline(ChannelDescriptor chnl,
                                   CommNode* pusher,
                                   CommNode* puller,
                                   int port)
{
    TRC("Creating Pipeline " << chnl);
    std::string bindAddr;
    std::string connAddr;
    if (port <= 0) {
        bindAddr = "inproc://" + chnl;
        connAddr = bindAddr;
    } else {
        std::string pusherHost = pusher->getAddress();
        std::string pullerHost = puller->getAddress();
        std::stringstream ssPort;
        ssPort << port;
        bindAddr = "tcp://" + pusherHost + ":" + ssPort.str();
        connAddr = "tcp://" + pullerHost + ";" + pusherHost + ":" + ssPort.str();
    }
    DBG("Binding: " << bindAddr << ", Connect from: " << connAddr);
    pusher->addConnection(chnl, new Pipeline(NN_PUSH, bindAddr));
    puller->addConnection(chnl, new Pipeline(NN_PULL, connAddr));

    if (protLayerClass != PIPELINE) { components.clear(); }
    components.push_back(pusher);
    components.push_back(puller);
    protLayerClass = PIPELINE;
}

void ProtocolLayer::createPubSub(ChannelDescriptor chnl,
                                 std::vector<CommNode*> publishers,
                                 std::vector<CommNode*> subscribers,
                                 int port)
{
    TRC("Creating PubSub " << chnl);
    std::string bindAddr;
    std::string connAddr;
    if (port <= 0) {
        bindAddr = "inproc://" + chnl;
        connAddr = bindAddr;
    } else {
        std::string pubHost = publishers.at(0)->getAddress();
        std::stringstream ssPort;
        ssPort << port;
        bindAddr = "tcp://*:" + ssPort.str();
        connAddr = "tcp://" + pubHost + ":" + ssPort.str();
    }
    DBG("Binding: " << bindAddr << ", Connect from: " << connAddr);
    //if ((publishers.size() > 1) && (subscribers.size() == 1)) {
    //    bindAddr.swap(connAddr);
    //}
    components.clear();
    for (auto & c: publishers) {
        c->addConnection(chnl, new PubSub(NN_PUB, bindAddr));
        components.push_back(c);
    }
    for (auto & c: subscribers) {
        c->addConnection(chnl, new PubSub(NN_SUB, connAddr));
        components.push_back(c);
    }
    protLayerClass = PUBSUB;
}

void ProtocolLayer::createSurvey(ChannelDescriptor chnl,
                                 CommNode * surveyor, std::vector<CommNode*> respondents,
                                 int port)
{
    TRC("Creating Survey " << chnl);
    std::string bindAddr;
    std::string connAddr;
    if (port <= 0) {
        bindAddr = "inproc://" + chnl;
        connAddr = bindAddr;
    } else {
        std::string surveyorHost = surveyor->getAddress();
        std::stringstream ssPort;
        ssPort << port;
        bindAddr = "tcp://*:" + ssPort.str();
        connAddr = "tcp://" + surveyorHost + ":" + ssPort.str();
    }
    DBG("Binding: " << bindAddr << ", Connect from: " << connAddr);
    components.clear();
    surveyor->addConnection(chnl, new Survey(NN_SURVEYOR,   bindAddr));
    components.push_back(surveyor);
    for (auto & c: respondents) {
        c->addConnection(chnl, new Survey(NN_RESPONDENT, connAddr));
        components.push_back(c);
    }
    protLayerClass = SURVEY;
}

void ProtocolLayer::createBus(ChannelDescriptor chnl,
                              std::vector<CommNode*> elements,
                              int port)
{
    TRC("Creating Bus " << chnl);
    std::string addr;
    std::vector<std::string> addresses;
    int i = 1;

    components.clear();
    for (auto & c: elements) {
        if (port <= 0) {
            std::stringstream ssNum;
            ssNum << i;
            addr += "inproc://" + chnl + "_" + ssNum.str();
        } else {
            std::string host = c->getAddress();
            std::stringstream ssPort;
            ssPort << port;
            addr = "tcp://" + host + ":" + ssPort.str();
        }
        DBG("Binding: " << addr << ", Connect from anywhere");
        Bus * conn = new Bus(NN_BUS,   addr);
        for (auto & adr: addresses) { conn->connectTo(adr); }
        addresses.push_back(addr);
        c->addConnection(chnl, conn);
        components.push_back(c);
        ++i;
    }
    protLayerClass = BUS;
}
