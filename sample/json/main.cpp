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

    MessageBase msg(JValue(content).val());
    Message<MsgBodyCMD> msgCmd("{\"header\": " + msg("header").str() + ", "
                               "\"body\": " + MsgBodyCMD(std::string("{\"cmd\": \"time 50\"}")).str() + "}");

    msg.dump();
    msgCmd.dump();

    DBG(msg.str());
    DBG(msgCmd.str());

    //------------------------------------------------------------

    std::ifstream cfgFile(argv[1]);
    std::stringstream buffer;
    buffer << cfgFile.rdbuf();

    JValue v;
    v.fromStr(buffer.str());

    //Config & cfg = Config::_();
    using Configuration::cfg;
    cfg.init(v.val());

    std::cerr << cfg.str() << std::endl;
    std::cerr << cfg.general.appName() << std::endl;
    std::cerr << cfg.network.masterNode() << std::endl;
    for (auto & kv : cfg.network.processingNodes()) {
        std::cerr << kv.first << ": " << kv.second << std::endl;
    }

    cfg.dump();

    DBG("Config::PATHBase: " << Config::PATHBase);


    return 0;
}
