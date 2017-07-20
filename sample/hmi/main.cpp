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

#include "hmipxy.h"

using Configuration::cfg;

Synchronizer synchro;

///////////////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[])
{
    Dbg::verbosityLevel = Dbg::TRACE_LEVEL;

    // Initialize configuration
    cfg.init(std::string(argv[1]));

    std::cerr << cfg.str() << std::endl;
    std::cerr << cfg.general.appName() << std::endl;
    std::cerr << cfg.network.masterNode() << std::endl;
    for (auto & kv : cfg.network.processingNodes()) {
        std::cerr << kv.first << ": " << kv.second << std::endl;
    }

    int startingPort = atoi(argv[2]);

    //------------------------------------------------------------

    std::string masterAddress = cfg.network.masterNode();
    DBG("MasterAddress is " << masterAddress);
    DBG("Connection port is " << startingPort);

    // Connection addresses and channel
    std::string bindAddr;
    std::string connAddr;
    std::string chnl;

    //-----------------------------------------------------------------
    // a. Create master node elements
    //-----------------------------------------------------------------
    HMIProxy * hmipxy = new HMIProxy("HMIProxy",  masterAddress, &synchro);

    //-----------------------------------------------------------------
    // b. Create component connections
    //-----------------------------------------------------------------

    // CHANNEL HMICMD - REQREP
    // - Out/In: QPFHMI/EvtMng
    chnl     = ChnlHMICmd;
    connAddr = "tcp://" + masterAddress + ":" + str::toStr<int>(startingPort);
    hmipxy->addConnection(chnl, new ReqRep(NN_REQ, connAddr));

    // START!
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    synchro.notify();

    // FOREVER
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    synchro.wait();
    std::cerr << "Exiting..." << std::endl;

    return 0;
}
