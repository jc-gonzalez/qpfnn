#include <iostream>

#include "protlayer.h"

#include "evtmng.h"
#include "datamng.h"
#include "logmng.h"
#include "tskorc.h"
#include "tskmng.h"
#include "tskage.h"

#include "message.h"
#include "channels.h"

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

    DBG(msg.str());

    EvtMng  evtMng("EvtMng",  "127.0.0.1");
    DataMng datMng("DataMng", "127.0.0.1");
    LogMng  logMng("LogMng",  "127.0.0.1");
    TskOrc  tskOrc("TskOrc",  "127.0.0.1");
    TskMng  tskMng("TskMng",  "127.0.0.1");

    std::vector<CommNode*> tskAgents;
    tskAgents.push_back(new TskAge("TskAgent1", "127.0.0.1"));
    tskAgents.push_back(new TskAge("TskAgent2", "127.0.0.1"));
    tskAgents.push_back(new TskAge("TskAgent3", "127.0.0.1"));

    auto concat = [](std::vector<CommNode*> a, std::vector<CommNode*> b)
        -> std::vector<CommNode*> { a.insert(a.end(), b.begin(), b.end()); return a;};

    std::vector<CommNode*> allCommNodes =
        concat(std::vector<CommNode*> {&datMng, &logMng, &tskOrc, &tskMng}, tskAgents);

    std::string channel;
    std::string address;

    //-----------------------------------------------------------------
    // Channel CMD - SURVEY
    // - Surveyor: EvtMng
    // - Respondent: QPFHMI DataMng LogMng, TskOrc TskMng TskAge*
    ProtocolLayer p1;
    p1.createSurvey(ChnlCmd, &evtMng, allCommNodes, 5555);

    //-----------------------------------------------------------------
    // Channel INDATA - PUBSUB
    // - Publisher: EvtMng
    // - Subscriber: DataMng TskOrc
    ProtocolLayer p2;
    p2.createPubSub(ChnlInData,
                   std::vector<CommNode*> {&evtMng},
                   std::vector<CommNode*> {&datMng, &tskOrc});

    //-----------------------------------------------------------------
    // Channel MONITORING - BUS
    // - Connected: EvtMng QPFHMI DataMng LogMng, TskOrc TskMng TskAge*
    ProtocolLayer p3;
    p3.createBus(ChnlMonit,
                 std::vector<CommNode*> {&datMng, &logMng, &tskOrc, &tskMng});

    //-----------------------------------------------------------------
    // Channel TASK-SCHEDULING - PUBSUB
    // - Publisher: TskOrc
    // - Subscriber: DataMng TskMng
    ProtocolLayer p4;
    p4.createPubSub(ChnlTskSched,
                    std::vector<CommNode*> {&tskOrc},
                    std::vector<CommNode*> {&datMng, &tskMng});

    //-----------------------------------------------------------------
    // Channel TASK-PROCESSING - PIPELINE
    // - Out/In: TskMng/TskAge*
    ProtocolLayer p5;
    for (auto & c: tskAgents) {
        p5.createPipeline(ChnlTskProc + "_" + c->getName(), &tskMng, c);
    }

    //-----------------------------------------------------------------
    // Channel TASK-MONITORING - SURVEY
    // - Surveyor: TskMng
    // - Respondent: TskAge*
    ProtocolLayer p6;
    p6.createSurvey(ChnlTskMonit, &tskMng, tskAgents, 5557);

    //-----------------------------------------------------------------
    // Channel TASK-REPORTING - PUBSUB
    // - Publisher: TskMng
    // - Subscriber: DataMng EvtMng QPFHMI
    ProtocolLayer p7;
    p7.createPubSub(ChnlTskRep,
                    std::vector<CommNode*> {&tskMng},
                    std::vector<CommNode*> {&datMng, &evtMng});

    // Define periodic message in  a certain channel at component level
    MessageString msgStr = msg.str();
    evtMng.periodicMsgInChannel(ChnlInData, 13, msgStr);

    // START!
    for (auto p: {p1, p2, p3, p4, p5, p6, p7}) { p.go(); }

    // FOREVER
    for(;;) {}

    return 0;
}
