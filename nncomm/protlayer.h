// -*- C++ -*-

#ifndef PROTLAYER_H
#define PROTLAYER_H

#include <vector>

#include "commnode.h"

class ProtocolLayer {
public:
    enum ProtocolLayerClass { UNDEFINED, PIPELINE, PUBSUB, SURVEY, BUS};
    ProtocolLayer();

    ProtocolLayerClass getClass();
    void go();

    void createPipeline(ChannelDescriptor chnl,
                        CommNode* pusher,
                        CommNode* puller,
                        int port = 0);
    void createPubSub(ChannelDescriptor chnl,
                      std::vector<CommNode*> publishers,
                      std::vector<CommNode*> subscribers,
                      int port = 0);
    void createSurvey(ChannelDescriptor chnl,
                      CommNode * surveyor, std::vector<CommNode*> respondents,
                      int port = 0);
    void createBus(ChannelDescriptor chnl,
                   std::vector<CommNode*> elements,
                   int port = 0);

private:
    std::vector<CommNode*> components;
    ProtocolLayerClass protLayerClass;
};

#endif
