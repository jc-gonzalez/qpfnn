// -*- C++ -*-

#ifndef PROTLAYER_H
#define PROTLAYER_H

#include <vector>

#include "commnode.h"

class ProtocolLayer {

public:

#define TLIST_OF_PROTLAYERCLASSES                               \
    T(UNDEFINED), T(PIPELINE), T(PUBSUB), T(SURVEY), T(BUS)

    ProtocolLayer();

#define T(x)  x
    enum ProtocolLayerClass { TLIST_OF_PROTLAYERCLASSES };
#undef T

    static const std::string ProtocolLayerClassName[];

    ProtocolLayerClass getClass();
    std::string getClassName();

    void createPipeline(ChannelDescriptor chnl,
                        CommNode* pusher,
                        CommNode* puller,
                        std::string pusherHost = std::string(),
                        std::string pullerHost = std::string(),
                        int port = 0);
    void createPubSub(ChannelDescriptor chnl,
                      std::vector<CommNode*> publishers,
                      std::vector<CommNode*> subscribers,
                      std::string publisherHost = std::string(),
                      int port = 0);
    void createSurvey(ChannelDescriptor chnl,
                      CommNode * surveyor, std::vector<CommNode*> respondents,
                      std::string surveyorHost = std::string(),
                      int port = 0);
    void createBus(ChannelDescriptor chnl,
                   std::vector<CommNode*> elements,
                   std::vector<std::string> addresses,
                   int port = 0);

private:
    std::vector<CommNode*> components;
    ProtocolLayerClass protLayerClass;
};

#endif
