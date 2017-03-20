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
                                             std::string thisHost,
                                             int startingPort)
{
    // Handy lambda function to concatenate vectors
    auto concat = [](std::vector<CommNode*> a, std::vector<CommNode*> b)
        -> std::vector<CommNode*> { a.insert(a.end(), b.begin(), b.end()); return a;};

    // Handy lambda to compute ports number, h=1:procHosts, i=0:agentsInHost
    // We will assume agentsInHost is < 10
    auto portnum = [](int start, int h, int i) -> int
        { return start + 10 * (h - 1) + i; };

    // Port to use for the connections (range starts with startingPort)
    std::vector<int> agPort;

    // Vector of agent names
    char sAgName[20];
    std::vector<std::string> agName;
    std::vector<std::string> agHost;

    //========================================
    // 1. Gather host information
    //========================================
    char hostname[100];
    gethostname(hostname, 100);
    //std::string thisHost(hostname);
    std::string masterAddress = cfg.network.masterNode();

    // Assuming a max. of 9 agents per proc. host, each prot.layer involving agents
    // uses a set of ports given by this number
    int protLayerPortStep = cfg.network.processingNodes().size() * 10;

    DBG("MasterAddress is " << masterAddress);

    //========================================
    // 2. Create the elements
    //========================================
    if (thisHost == masterAddress) {
        // a. Create master node elements
        m.evtMng = new EvtMng  ("EvtMng",  masterAddress, &synchro);
        m.datMng = new DataMng ("DataMng", masterAddress, &synchro);
        m.logMng = new LogMng  ("LogMng",  masterAddress, &synchro);
        m.tskOrc = new TskOrc  ("TskOrc",  masterAddress, &synchro);
        m.tskMng = new TskMng  ("TskMng",  masterAddress, &synchro);
        // b. Fill agents vector with as many zeroes as total number of agents
        int h = 1;
        for (auto & kv : cfg.network.processingNodes()) {
            for (unsigned int i = 0; i < kv.second; ++i) {
                sprintf(sAgName, "TskAgent_%02d_%02d", h, i + 1);
                ag.push_back(0);
                agName.push_back(std::string(sAgName));
                agPort.push_back(portnum(startingPort, h, i));
                agHost.push_back(kv.first);
            }
            ++h;
        }
    } else {
        // a. There are no master nodes in this host
        m = MasterNodeElements {0, 0, 0, 0, 0};
        // b. Fill agents vector with as agents for this host
        int h = 1;
        for (auto & kv : cfg.network.processingNodes()) {
            if (thisHost == kv.first) {
                int numOfTskAgents = kv.second;
                for (unsigned int i = 0; i < numOfTskAgents; ++i) {
                    sprintf(sAgName, "TskAgent_%02d_%02d", h, i + 1);
                    ag.push_back(new TskAge(sAgName, thisHost, &synchro));
                    agName.push_back(std::string(sAgName));
                    agPort.push_back(startingPort + 100 * (h - 1) + i);
                    agHost.push_back(kv.first);
                }
            }
            ++h;
        }
    }

    //========================================
    // 3. Create the protocol layers
    //========================================
    std::vector<CommNode*> allCommNodes =
        concat(std::vector<CommNode*> {m.datMng, m.logMng, m.tskOrc, m.tskMng}, ag);

    ProtocolLayer * p = 0;
    std::vector<ProtocolLayer *> protocolLayers;

    //-----------------------------------------------------------------
    // Channel CMD - SURVEY
    // - Surveyor: EvtMng
    // - Respondent: QPFHMI DataMng LogMng, TskOrc TskMng TskAge*
    p = new ProtocolLayer;
    p->createSurvey(ChnlCmd,
                    m.evtMng,
                    allCommNodes,
                    masterAddress);
    protocolLayers.push_back(p);

    //-----------------------------------------------------------------
    // Channel INDATA -  PUBSUB
    // - Publisher: EvtMng
    // - Subscriber: DataMng TskOrc
    p = new ProtocolLayer;
    p->createPubSub(ChnlInData,
                    std::vector<CommNode*> {m.evtMng},
                    std::vector<CommNode*> {m.datMng, m.tskOrc},
                    masterAddress);
    protocolLayers.push_back(p);

    //-----------------------------------------------------------------
    // Channel TASK-SCHEDULING - PUBSUB
    // - Publisher: TskOrc
    // - Subscriber: DataMng TskMng
    p = new ProtocolLayer;
    p->createPubSub(ChnlTskSched,
                    std::vector<CommNode*> {m.tskOrc},
                    std::vector<CommNode*> {m.datMng, m.tskMng},
                    masterAddress);
    protocolLayers.push_back(p);

    //-----------------------------------------------------------------
    // Channel TASK-REQUEST - PIPELINE
    // - Out/In: TskAge*/TskMng
    int k = 0;
    for (auto & c: ag) {
        p = new ProtocolLayer;
        p->createPipeline(ChnlTskRqst + "_" + agName.at(k),
                          c,
                          m.tskMng,
                          agHost.at(k),
                          masterAddress,
                          agPort.at(k));
        protocolLayers.push_back(p);
        ++k;
    }

    //-----------------------------------------------------------------
    // Channel TASK-PROCESSING - PIPELINE
    // - Out/In: TskMng/TskAge*
    k = 0;
    for (auto & c: ag) {
        p = new ProtocolLayer;
        p->createPipeline(ChnlTskProc + "_" + agName.at(k),
                          m.tskMng,
                          c,
                          masterAddress,
                          agHost.at(k),
                          agPort.at(k) + protLayerPortStep);
        protocolLayers.push_back(p);
        ++k;
    }

    //-----------------------------------------------------------------
    // Channel TASK-REPORTING - PIPELINE
    // - Out/In: TskAge*/TskMng
    k = 0;
    for (auto & c: ag) {
        p = new ProtocolLayer;
        p->createPipeline(ChnlTskRep + "_" + agName.at(k),
                          c,
                          m.tskMng,
                          agHost.at(k),
                          masterAddress,
                          agPort.at(k) + 2 * protLayerPortStep);
        protocolLayers.push_back(p);
        ++k;
    }

    //-----------------------------------------------------------------
    // Channel TASK-REPORTING-DISTRIBUTION - PUBSUB
    // - Publisher: TskMng
    // - Subscriber: DataMng EvtMng QPFHMI
    p = new ProtocolLayer;
    p->createPubSub(ChnlTskRepDist,
                    std::vector<CommNode*> {m.tskMng},
                    std::vector<CommNode*> {m.datMng, m.evtMng},
                    masterAddress);
    protocolLayers.push_back(p);

    // Append event manager to list of nodes
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
        createElementsNetwork(masterNodeElems, agentsNodes, std::string(argv[2]), 55000);

    // START!
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    synchro.notify();

    // FOREVER
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    synchro.wait();
    std::cerr << "Exiting..." << std::endl;

    return 0;
}
