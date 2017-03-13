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
    // 1. Create the elements
    //========================================
    m.evtMng = new EvtMng  ("EvtMng",  masterAddress, &synchro);
    m.datMng = new DataMng ("DataMng", masterAddress, &synchro);
    m.logMng = new LogMng  ("LogMng",  masterAddress, &synchro);
    m.tskOrc = new TskOrc  ("TskOrc",  masterAddress, &synchro);
    m.tskMng = new TskMng  ("TskMng",  masterAddress, &synchro);

    ag.push_back(new TskAge("TskAgent1", agentsAddress, &synchro));
    ag.push_back(new TskAge("TskAgent2", agentsAddress, &synchro));
    ag.push_back(new TskAge("TskAgent3", agentsAddress, &synchro));

    //========================================
    // 2. Create the connection channels
    //========================================

    auto concat = [](std::vector<CommNode*> a, std::vector<CommNode*> b)
        -> std::vector<CommNode*> { a.insert(a.end(), b.begin(), b.end()); return a;};

    std::vector<CommNode*> allMainNodes =
        std::vector<CommNode*> {m.datMng, m.logMng, m.tskOrc, m.tskMng};

    //-----------------------------------------------------------------
    // Channel CMD - SURVEY
    // - Surveyor: EvtMng
    // - Respondent: QPFHMI DataMng LogMng, TskOrc TskMng
    ProtocolLayer p1;
    p1.createSurvey(ChnlCmd, m.evtMng, allMainNodes);

    //-----------------------------------------------------------------
    // Channel INDATA -  PUBSUB
    // - Publisher: EvtMng
    // - Subscriber: DataMng TskOrc
    ProtocolLayer p2;
    p2.createPubSub(ChnlInData,
                   std::vector<CommNode*> {m.evtMng},
                   std::vector<CommNode*> {m.datMng, m.tskOrc});

    //-----------------------------------------------------------------
    // Channel TASK-SCHEDULING - PUBSUB
    // - Publisher: TskOrc
    // - Subscriber: DataMng TskMng
    ProtocolLayer p3;
    p3.createPubSub(ChnlTskSched,
                    std::vector<CommNode*> {m.tskOrc},
                    std::vector<CommNode*> {m.datMng, m.tskMng});

    //-----------------------------------------------------------------
    // Channel TASK-REQUEST - PIPELINEs
    // - Out/In: TskAge*/TskMng
    ProtocolLayer p4;
    for (auto & c: ag) {
        p4.createPipeline(ChnlTskRqst + "_" + dynamic_cast<Component*>(c)->getName(), m.tskMng, c);
    }

    //-----------------------------------------------------------------
    // Channel TASK-PROCESSING - PIPELINEs
    // - Out/In: TskMng/TskAge*
    ProtocolLayer p5;
    for (auto & c: ag) {
        p5.createPipeline(ChnlTskProc + "_" + dynamic_cast<Component*>(c)->getName(), c, m.tskMng);
    }

    //-----------------------------------------------------------------
    // Channel TASK-REPORTING - PUBSUB
    // - Publisher: TskAge*
    // - Subscriber: TskMng
    ProtocolLayer p6;
    p6.createPubSub(ChnlTskRep,
                    ag,
                    std::vector<CommNode*> {m.tskMng}, 5555);

    //-----------------------------------------------------------------
    // Channel TASK-REPORTING - PUBSUB
    // - Publisher: TskMng
    // - Subscriber: DataMng EvtMng QPFHMI
    ProtocolLayer p7;
    p7.createPubSub(ChnlTskRep,
                    std::vector<CommNode*> {m.tskMng},
                    std::vector<CommNode*> {m.datMng, m.evtMng});

    allMainNodes.push_back(m.evtMng);

    return concat(allMainNodes, ag);
}

///////////////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[])
{
    MessageString content("{\"header\": {\"source\": \"evtmng\", "
                          "              \"target\": \"tskorc\", "
                          "              \"type\": \"MSG_MONIT_RQST\"},"
                          "\"data\": {\"rqst\": \"GIMMEALL\", "
                          "           \"when\": \"NOW\" }}");
    /*
    Message msg(content);

    json hdr = msg["header"];
    DBG(msg.str());
    DBG(JValue(msg["header"]).str());
    DBG(JValue(msg.hdr()).str());
    DBG(msg.hdrStr());
    DBG(JValue(msg["data"]).str());
    DBG(JValue(msg.data()).str());

    msg.setHeader("0001", "MSG_DATA", 1, "SRC", "TGT", "NOW", "TOMORROW", "IN-TWO-YEARS", 0,
                  std::vector<std::string> {std::string("one"), std::string("two"), std::string("three")});

    msg.set("data1", "hola");
    msg.set("data2", 34.2);
    msg.set("data1", 55);
    msg.set("data3", "adios");
    msg.data()["data4"] = json("adios");
    msg["data"]["data4"] = json("adios");

    DBG(msg.str());

    msg.newTargetIs("four");
    */

    MessageBase msg(JValue(content).val());
    Message<MsgBodyCMD> msgCmd("{\"header\": " + msg("header").str() + ", "
                               "\"body\": " + MsgBodyCMD(std::string("{\"cmd\": \"time 50\"}")).str() + "}");

    msg.dump();
    msgCmd.dump();

    DBG(msg.str());
    DBG(msgCmd.str());

    //------------------------------------------------------------

    std::ifstream cfgFile("/home/jcgonzalez/file.cfg");
    std::stringstream buffer;
    buffer << cfgFile.rdbuf();

    JValue v;
    v.fromStr(buffer.str());

    Config cfg(v.val());
    std::cerr << cfg.str() << std::endl;
    std::cerr << cfg.general.appName() << std::endl;
    std::cerr << cfg.network.masterNode() << std::endl;
    for (auto & kv : cfg.network.processingNodes()) {
        std::cerr << kv.first << ": " << kv.second << std::endl;
    }

    //cfg.dump();

    //------------------------------------------------------------

    MasterNodeElements masterNodeElems;
    std::vector<CommNode*> agentsNodes;

    std::vector<CommNode*> allCommNodes =
        createElementsNetwork(masterNodeElems, agentsNodes,
                              "127.0.0.1",     "127.0.0.1");
    // std::vector<ProtocolLayer> network =

    // Define periodic message in  a certain channel at component level
    MessageString msgStr = msg.str();
    masterNodeElems.evtMng->periodicMsgInChannel(ChnlInData, 13, msgStr);

    // START!
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    synchro.notify();

    // FOREVER
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    synchro.wait();

    return 0;
}
