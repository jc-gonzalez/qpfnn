#include <iostream>
#include <fstream>
#include <sstream>

#include "protlayer.h"

#include "evtmng.h"
#include "datamng.h"
#include "logmng.h"
#include "tskorc.h"
#include "tskmng.h"
#include "tskage.h"

#include "message.h"
#include "channels.h"
#include "config.h"

using Configuration::cfg;

struct MasterNodeElements {
    EvtMng  * evtMng;
    DataMng * datMng;
    LogMng  * logMng;
    TskOrc  * tskOrc;
    TskMng  * tskMng;
};

typedef MasterNodeElements * MasterNodeElementsPtr;

Synchronizer synchro;

//----------------------------------------------------------------------
// createElementsNetwork
//----------------------------------------------------------------------
std::vector<CommNode*> createElementsNetwork(MasterNodeElements & m,
                                             std::vector<CommNode*>  & ag,
                                             std::string masterAddress,
                                             std::string agentsAddress)
{
    //========================================
    // 0. Gather host information
    //========================================
    char hostname[100];
    gethostname(hostname, 100);
    std::string thisHost(hostname);

    //========================================
    // 1. Create the elements
    //========================================
    if (thisHost == cfg.network.masterNode()) {
        // Create master node elements
        m.evtMng = new EvtMng  ("EvtMng",  masterAddress, &synchro);
        m.datMng = new DataMng ("DataMng", masterAddress, &synchro);
        m.logMng = new LogMng  ("LogMng",  masterAddress, &synchro);
        m.tskOrc = new TskOrc  ("TskOrc",  masterAddress, &synchro);
        m.tskMng = new TskMng  ("TskMng",  masterAddress, &synchro);
    } else {
        for (auto & kv : cfg.network.processingNodes()) {
            if (thisHost == kv.first) {
                int numOfTskAgents = kv.second;
                char sAgName[20];
                for (unsigned int i = 0; i < numOfTskAgents; ++i) {
                    sprintf(sAgName, "TskAgent%02dd", i);
                    ag.push_back(new TskAge(sAgName, thisHost, &synchro));
                }
            }
        }

    }

    //========================================
    // 2. Create the connection channels
    //========================================
    auto concat = [](std::vector<CommNode*> a, std::vector<CommNode*> b)
        -> std::vector<CommNode*> { a.insert(a.end(), b.begin(), b.end()); return a;};

    std::vector<CommNode*> allCommNodes =
        concat(std::vector<CommNode*> {m.datMng, m.logMng, m.tskOrc, m.tskMng}, ag);

    ProtocolLayer * p;
    std::vector<ProtocolLayer *> protocolLayers;

    //-----------------------------------------------------------------
    // Channel CMD - SURVEY
    // - Surveyor: EvtMng
    // - Respondent: QPFHMI DataMng LogMng, TskOrc TskMng TskAge*
    p = new ProtocolLayer;
    p->createSurvey(ChnlCmd, m.evtMng, allCommNodes);
    protocolLayers.push_back(p);

    //-----------------------------------------------------------------
    // Channel INDATA -  PUBSUB
    // - Publisher: EvtMng
    // - Subscriber: DataMng TskOrc
    p = new ProtocolLayer;
    p->createPubSub(ChnlInData,
                   std::vector<CommNode*> {m.evtMng},
                   std::vector<CommNode*> {m.datMng, m.tskOrc});
    protocolLayers.push_back(p);

    //-----------------------------------------------------------------
    // Channel TASK-SCHEDULING - PUBSUB
    // - Publisher: TskOrc
    // - Subscriber: DataMng TskMng
    p = new ProtocolLayer;
    p->createPubSub(ChnlTskSched,
                    std::vector<CommNode*> {m.tskOrc},
                    std::vector<CommNode*> {m.datMng, m.tskMng});
    protocolLayers.push_back(p);

    //-----------------------------------------------------------------
    // Channel TASK-REQUEST - PIPELINE
    // - Out/In: TskAge*/TskMng
    for (auto & c: ag) {
        p = new ProtocolLayer;
        p->createPipeline(ChnlTskRqst + "_" + dynamic_cast<Component*>(c)->getName(), c, m.tskMng);
        protocolLayers.push_back(p);
    }

    //-----------------------------------------------------------------
    // Channel TASK-PROCESSING - PIPELINE
    // - Out/In: TskMng/TskAge*
    for (auto & c: ag) {
        p = new ProtocolLayer;
        p->createPipeline(ChnlTskProc + "_" + dynamic_cast<Component*>(c)->getName(), m.tskMng, c);
        protocolLayers.push_back(p);
    }

    //-----------------------------------------------------------------
    // Channel TASK-REQUEST - PIPELINE
    // - Out/In: TskAge*/TskMng
    int port = 55000;
    for (auto & c: ag) {
        p = new ProtocolLayer;
        p->createPipeline(ChnlTskRep + "_" + dynamic_cast<Component*>(c)->getName(), c, m.tskMng, port);
        protocolLayers.push_back(p);
        ++port;
    }

    //-----------------------------------------------------------------
    // Channel TASK-REPORTING - PUBSUB
    // - Publisher: TskMng
    // - Subscriber: DataMng EvtMng QPFHMI
    p = new ProtocolLayer;
    p->createPubSub(ChnlTskRepDist,
                    std::vector<CommNode*> {m.tskMng},
                    std::vector<CommNode*> {m.datMng, m.evtMng});
    protocolLayers.push_back(p);

    allCommNodes.push_back(m.evtMng);

    return allCommNodes;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[])
{
    std::string cfgFileName(argv[1]);
    std::ifstream cfgFile(cfgFileName);
    std::stringstream buffer;
    buffer << cfgFile.rdbuf();

    JValue v;
    v.fromStr(buffer.str());

    cfg.init(v.val());

    std::cerr << cfg.str() << std::endl;
    std::cerr << cfg.general.appName() << std::endl;
    std::cerr << cfg.network.masterNode() << std::endl;
    for (auto & kv : cfg.network.processingNodes()) {
        std::cerr << kv.first << ": " << kv.second << std::endl;
    }

    cfg.dump();

    DBG("Config::PATHBase: " << Config::PATHBase);

    //------------------------------------------------------------

    MasterNodeElements masterNodeElems;
    std::vector<CommNode*> agentsNodes;

    std::vector<CommNode*> allCommNodes =
        createElementsNetwork(masterNodeElems, agentsNodes,
                              "127.0.0.1",     "127.0.0.1");
    // std::vector<ProtocolLayer> network =

    // Define periodic message in  a certain channel at component level
    //MessageString msgStr = msg.str();
    //masterNodeElems.evtMng->periodicMsgInChannel(ChnlInData, 13, msgStr);

    // START!
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    synchro.notify();

    // FOREVER
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    synchro.wait();
    std::cerr << "Exiting..." << std::endl;

    return 0;
}
