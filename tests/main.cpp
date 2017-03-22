#include <iostream>
#include <fstream>
#include <sstream>

#include "evtmng.h"
#include "datamng.h"
#include "logmng.h"
#include "tskorc.h"
#include "tskmng.h"
#include "tskage.h"

#include "survey.h"
#include "pubsub.h"
#include "reqrep.h"

#include "message.h"
#include "channels.h"
#include "config.h"
#include "str.h"


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
void createElementsNetwork(MasterNodeElements & m,
                           std::vector<CommNode*>  & ag,
                           std::string thisHost,
                           int startingPort)
{
    // Handy lambda to compute ports number, h=1:procHosts, i=0:agentsInHost
    // We will assume agentsInHost is < 10
    auto portnum = [](int start, int h, int i) -> int
        { return start + 10 * (h - 1) + i; };

    //======================================================================
    // 1. Gather host information
    //======================================================================

    std::string masterAddress = cfg.network.masterNode();
    bool isMasterHost = (thisHost == masterAddress);

    DBG("MasterAddress is " << masterAddress);

    //======================================================================
    // 2. Create the elements and connections for the host we are running on
    //======================================================================

    // Agent names, hosts and ports (port range starts with startingPort)
    char sAgName[20];
    std::vector<std::string> agName;
    std::vector<std::string> agHost;
    std::vector<int> agPortCmd;
    std::vector<int> agPortTsk;

    // Connection addresses and channel
    std::string bindAddr;
    std::string connAddr;
    std::string chnl;

    if (isMasterHost) {

        //-----------------------------------------------------------------
        // a. Create master node elements
        //-----------------------------------------------------------------
        m.evtMng = new EvtMng  ("EvtMng",  masterAddress, &synchro);
        m.datMng = new DataMng ("DataMng", masterAddress, &synchro);
        m.logMng = new LogMng  ("LogMng",  masterAddress, &synchro);
        m.tskOrc = new TskOrc  ("TskOrc",  masterAddress, &synchro);
        m.tskMng = new TskMng  ("TskMng",  masterAddress, &synchro);

        //-----------------------------------------------------------------
        // b. Fill agent vectors for all the declared processing hosts
        //-----------------------------------------------------------------
        int h = 1;
        for (auto & kv : cfg.network.processingNodes()) {
            for (unsigned int i = 0; i < kv.second; ++i) {
                sprintf(sAgName, "TskAgent_%02d_%02d", h, i + 1);
                agName.push_back(std::string(sAgName));
                agPortCmd.push_back(portnum(startingPort, h, i));
                agPortTsk.push_back(portnum(startingPort+1000, h, i));
            }
            ++h;
        }

        //-----------------------------------------------------------------
        // c. Create component connections
        //-----------------------------------------------------------------

        // CHANNEL CMD - SURVEY
        // - Surveyor: EvtMng
        // - Respondent: QPFHMI DataMng LogMng, TskOrc TskMng TskAge*
        chnl     = ChnlCmd;
        bindAddr = "inproc://" + chnl;
        connAddr = bindAddr;
        m.evtMng->addConnection(chnl, new Survey(NN_SURVEYOR, bindAddr));
        for (auto & c : std::vector<CommNode*> {m.datMng, m.logMng, m.tskOrc, m.tskMng}) {
            c->addConnection(chnl, new Survey(NN_RESPONDENT, connAddr));
        }

        // CHANNEL INDATA -  PUBSUB
        // - Publisher: EvtMng
        // - Subscriber: DataMng TskOrc
        chnl     = ChnlInData;
        bindAddr = "inproc://" + chnl;
        connAddr = bindAddr;
        m.evtMng->addConnection(chnl, new PubSub(NN_PUB, bindAddr));
        for (auto & c: std::vector<CommNode*> {m.datMng, m.tskOrc}) {
            c->addConnection(chnl, new PubSub(NN_SUB, connAddr));
        }

        // CHANNEL TASK-SCHEDULING - PUBSUB
        // - Publisher: TskOrc
        // - Subscriber: DataMng TskMng
        chnl     = ChnlTskSched;
        bindAddr = "inproc://" + chnl;
        connAddr = bindAddr;
        m.tskOrc->addConnection(chnl, new PubSub(NN_PUB, bindAddr));
        for (auto & c: std::vector<CommNode*> {m.datMng, m.tskMng}) {
            c->addConnection(chnl, new PubSub(NN_SUB, connAddr));
        }

        // CHANNEL TASK-PROCESSING - REQREP
        // - Out/In: TskAge*/TskMng
        h = 0;
        for (auto & p : agPortTsk) {
            chnl = ChnlTskProc + "_" + agName.at(h);
            bindAddr = "tcp://" + masterAddress + ":" + str::toStr<int>(p);
            m.tskMng->addConnection(chnl, new ReqRep(NN_REP, bindAddr));
            ++h;
        }

        // CHANNEL TASK-REPORTING-DISTRIBUTION - PUBSUB
        // - Publisher: TskMng
        // - Subscriber: DataMng EvtMng QPFHMI
        chnl     = ChnlTskRepDist;
        bindAddr = "inproc://" + chnl;
        connAddr = bindAddr;
        m.tskMng->addConnection(chnl, new PubSub(NN_PUB, bindAddr));
        for (auto & c: std::vector<CommNode*> {m.datMng, m.evtMng}) {
            c->addConnection(chnl, new PubSub(NN_SUB, connAddr));
        }

    } else {

        //-----------------------------------------------------------------
        // a. Fill agents vector with as agents for this host
        //-----------------------------------------------------------------
        int h = 1;
        for (auto & kv : cfg.network.processingNodes()) {
            if (thisHost == kv.first) {
                int numOfTskAgents = kv.second;
                for (unsigned int i = 0; i < numOfTskAgents; ++i) {
                    sprintf(sAgName, "TskAgent_%02d_%02d", h, i + 1);
                    ag.push_back(new TskAge(sAgName, thisHost, &synchro));
                    agName.push_back(std::string(sAgName));
                    agPortTsk.push_back(portnum(startingPort, h, i));
                }
            }
            ++h;
        }

        //-----------------------------------------------------------------
        // b. Create agent connections
        //-----------------------------------------------------------------

        // CHANNEL TASK-PROCESSING - REQREP
        // - Out/In: TskAge*/TskMng
        h = 0;
        for (auto & a : ag) {
            chnl = ChnlTskProc + "_" + agName.at(h);
            connAddr = "tcp://" + masterAddress + ":" + str::toStr<int>(agPortTsk.at(h));
            ReqRep * r = new ReqRep(NN_REQ, connAddr);
            a->addConnection(chnl, r);
            ++h;
        }

    }
}

///////////////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[])
{
    // Initialize configuration
    cfg.init(std::string(argv[1]));

    std::cerr << cfg.str() << std::endl;
    std::cerr << cfg.general.appName() << std::endl;
    std::cerr << cfg.network.masterNode() << std::endl;
    for (auto & kv : cfg.network.processingNodes()) {
        std::cerr << kv.first << ": " << kv.second << std::endl;
    }
    // cfg.dump();
    // DBG("Config::PATHBase: " << Config::PATHBase);

    //------------------------------------------------------------

    MasterNodeElements masterNodeElems;
    std::vector<CommNode*> agentsNodes;

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
