/******************************************************************************
 * File:    deployer.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.QPF.Deployer
 *
 * Version:  1.2
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement Deployer class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   none
 *
 * Files read / modified:
 *   none
 *
 * History:
 *   See <Changelog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#include "deployer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "survey.h"
#include "pubsub.h"
#include "reqrep.h"

#include "message.h"
#include "channels.h"
#include "config.h"
#include "str.h"
#include "dbg.h"

using Configuration::cfg;

#define MINOR_SYNC_DELAY_MS    500

#define DEFAULT_INITIAL_PORT   50000

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

Deployer * deployerCatcher;

//----------------------------------------------------------------------
// Ancillary signal handler
//----------------------------------------------------------------------
void signalCatcher(int s)
{
    deployerCatcher->actionOnSigInt();
}

//----------------------------------------------------------------------
// Constructor: Deployer
//----------------------------------------------------------------------
Deployer::Deployer(int argc, char *argv[])
{
    // Get host info
    getHostnameAndIp(currentHostName, currentHostAddr);
    setInitialPort(DEFAULT_INITIAL_PORT);

    //== Change value for delay between peer nodes launches (default: 50000us)
    if ((!processCmdLineOpts(argc, argv)) || (cfgFileName.empty())) {
        usage(EXIT_FAILURE);
    }

    //== ReadConfiguration
    readConfiguration();

    //== Install signal handler
    deployerCatcher = this;
    installSignalHandlers();
}

//----------------------------------------------------------------------
// Destructor: Deployer
//----------------------------------------------------------------------
Deployer::~Deployer()
{
}

//----------------------------------------------------------------------
// Method: run
// Launches the system components and starts the system
//----------------------------------------------------------------------
int Deployer::run()
{
    // Greetings...
    sayHello();

    // System components creation and setup
    createElementsNetwork();

    // START!
    delay(MINOR_SYNC_DELAY_MS);
    synchro.notify();

    // FOREVER
    delay(MINOR_SYNC_DELAY_MS);
    synchro.wait();

    // Bye, bye
    INFO("Exiting...");
    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------
// Method: usage
// Shows usage information
//----------------------------------------------------------------------
bool Deployer::usage(int code)
{
    Dbg::verbosityLevel = Dbg::INFO_LEVEL;

    INFO("Usage: " << exeName << "  -c configFile "
         "[ -p initialPort ] [-H hostName ] [ -I ipAddress ] [ -v ] [ -h ]\n"
         "where:\n"
         "\t-c cfgFile          System is reconfigured with configuration in\n"
         "\t                    file cfgFile (configuration is then saved to DB).\n"
         "\t-p initialPort      Set initial port for system set up (default:"
         << DEFAULT_INITIAL_PORT << ").\n\n"
         "\t-H hostName         Set the host name (by default the program takes\n"
         "\t                    this information from the system).\n"
         "\t-I IPaddress        Set the host IP address (by default the program takes\n"
         "\t                    this information from the system).\n"
         "\t-v                  Increases verbosity (default:silent operation).\n\n"
         "\t-h                  Shows this help message.\n");

    exit(code);
}

//----------------------------------------------------------------------
// Method: processCmdLineOpts
// Processes command line options to configure execution
//----------------------------------------------------------------------
bool Deployer::processCmdLineOpts(int argc, char * argv[])
{
    bool retVal = true;
    int exitCode = EXIT_FAILURE;

    exeName = std::string(argv[0]);

    int opt;
    while ((opt = getopt(argc, argv, "hvp:c:H:I:")) != -1) {
        switch (opt) {
        case 'v':
            Dbg::verbosityLevel++;
            break;
        case 'c':
            setConfigFile(std::string(optarg));
            break;
        case 'p':
            setInitialPort(atoi(optarg));
            break;
        case 'H':
            setCurrentHostName(std::string(optarg));
            break;
        case 'I':
            setCurrentHostAddr(std::string(optarg));
            break;
        case 'h':
            exitCode = EXIT_SUCCESS;
        default: /* '?' */
            usage(exitCode);
        }
    }

    return retVal;
}

//----------------------------------------------------------------------
// Method: setConfigFile
// Sets the name of the configuration file to be used
//----------------------------------------------------------------------
void Deployer::setConfigFile(std::string fName)
{
    cfgFileName = fName;
}

//----------------------------------------------------------------------
// Method: setInitialPort
// Sets the initial port for communications set up
//----------------------------------------------------------------------
void Deployer::setInitialPort(int port)
{
    initialPort = port;
}

//----------------------------------------------------------------------
// Method: setCurrentHostName
// Set the host name the current host
//----------------------------------------------------------------------
void Deployer::setCurrentHostName(std::string host)
{
    currentHostName = host;
}

//----------------------------------------------------------------------
// Method: setCurrentHostAddr
// Set the address (IP) of the current host
//----------------------------------------------------------------------
void Deployer::setCurrentHostAddr(std::string addr)
{
    currentHostAddr = addr;
}

//----------------------------------------------------------------------
// Method: readConfiguration
// Retrieves the configuration for the execution of the system (from
// a disk file or from the internal database)
//----------------------------------------------------------------------
void Deployer::readConfiguration()
{
    // Initialize configuration
    cfg.setCurrentHostAddress(currentHostAddr);
    cfg.init(cfgFileName);
    DBG("Master host: " << cfg.network.masterNode());
    DBG("Current host: " << currentHostAddr << "/"
        << cfg.currentHostAddr);
    DBG("Running in a "
        << std::string(cfg.weAreOnMaster ? "MASTER" : "PROCESSING")
        << " Host.");

    TRC(cfg.str());
    TRC(cfg.general.appName());
    TRC(cfg.network.masterNode());
    for (auto & kv : cfg.network.processingNodes()) {
        TRC(kv.first << ": " << kv.second);
    }
    //cfg.dump();
    TRC("Config::PATHBase....: " << Config::PATHBase);
    TRC("Config::PATHSession.: " << Config::PATHSession);

    // Ensure paths for the execution are available and readu
    assert(existsDir(Config::PATHBase));
    assert(existsDir(Config::PATHBin));
    std::vector<std::string> runPaths {
            Config::PATHSession,
            Config::PATHLog,
            Config::PATHRlog,
            Config::PATHTmp,
            Config::PATHTsk,
            Config::PATHMsg };
    for (auto & p : runPaths) {
        TRC(p);
        if (mkdir(p.c_str(), Config::PATHMode) != 0) {
            std::perror(("mkdir " + p).c_str());
            exit(EXIT_FAILURE);
        }
    }
    Log::setLogBaseDir(Config::PATHSession);

}

//----------------------------------------------------------------------
// Method: delay
// Waits for a small time lapse for system sync
//----------------------------------------------------------------------
int Deployer::delay(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

//----------------------------------------------------------------------
// Method: sayHello
// Shows a minimal title and build id for the application
//----------------------------------------------------------------------
void Deployer::sayHello()
{
    std::string buildId(BUILD_ID);
    if (buildId.empty()) {
        char buf[20];
        sprintf(buf, "%ld", (long)(time(0)));
        buildId = std::string(buf);
    }

    const std::string hline("----------------------------------------"
                            "--------------------------------------");
    INFO(hline << '\n'
         << " " << APP_NAME << " - " << APP_LONG_NAME << '\n'
         << " " << APP_DATE << " - "
         << APP_RELEASE << " Build " << buildId << '\n'
         << " Started at " << currentHostName << " [" << currentHostAddr << "]\n"
         << hline << std::endl);
}

//----------------------------------------------------------------------
// Method: getHostnameAndIp
// Gets from the system the host name and ip address of the host
//----------------------------------------------------------------------
void Deployer::getHostnameAndIp(std::string & hName, std::string & ipAddr)
{
    const int MaxHostNameLen = 100;
    const int MaxAddrStringLen = 512;
    char hostname[MaxHostNameLen];
    char addrString[MaxAddrStringLen];
    struct hostent * hostPtr = 0;
    int errorNum;

    // Get hostname from the system
    gethostname(hostname, MaxHostNameLen);

    // If necessary, get the domain to form the canonical name.
    if (NULL == strchr(hostname, (int)'.')) {
        int x;
        hostPtr = gethostbyname(hostname);

        if (!strchr(hostPtr->h_name, '.')) {
            if (hostPtr->h_aliases) {
                for (x = 0; hostPtr->h_aliases[x]; ++x) {
                    if (strchr(hostPtr->h_aliases[x], '.') &&
                        (!strncasecmp(hostPtr->h_aliases[x],
                                      hostPtr->h_name,
                                      strlen(hostPtr->h_name)))) {
                        strcpy(hostname, hostPtr->h_aliases[x]);
                    }
                }
            }
        } else {
            strcpy(hostname, hostPtr->h_name);
        }
    }

    hName = std::string(hostname);

    //hostPtr = getipnodebyname(hostname, AF_INET,
    //                        (AI_ADDRCONFIG | AI_V4MAPPED), &errorNum);

    struct addrinfo *ai;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; /* IPv4 address family */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = (AI_ADDRCONFIG | AI_V4MAPPED); //AI_DEFAULT;

    if (getaddrinfo(hostname, NULL, &hints, &ai)) {
        printf("getaddrinfo() failed\n");
    } else {
        //if (inet_ntop(ai->ai_family, ai->ai_addr, addrString, 512))
        if (0 == getnameinfo(ai->ai_addr, sizeof(struct sockaddr),
                             addrString, 512, NULL, 0, NI_NUMERICHOST))
            ipAddr = std::string(addrString);
    }
    freeaddrinfo(ai);
}

//----------------------------------------------------------------------
// Method: actionOnSigInt
// Actions to be performed when capturing SigInt
//----------------------------------------------------------------------
void Deployer::actionOnSigInt()
{
}

//----------------------------------------------------------------------
// Method: existsDir
// Removes old log and msg files
//----------------------------------------------------------------------
bool Deployer::existsDir(std::string pathName)
{
    struct stat sb;
    return (stat(pathName.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

//----------------------------------------------------------------------
// Method: installSignalHandlers
// Install signal handlers
//----------------------------------------------------------------------
void Deployer::installSignalHandlers()
{
    sigIntHandler.sa_handler = signalCatcher;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
}

//----------------------------------------------------------------------
// createElementsNetwork
//----------------------------------------------------------------------
void Deployer::createElementsNetwork()
{
    // Handy lambda to compute ports number, h=1:procHosts, i=0:agentsInHost
    // We will assume agentsInHost is < 10
    auto portnum = [](int start, int h, int i) -> int
        { return start + 10 * (h - 1) + i; };

    //======================================================================
    // 1. Gather host information
    //======================================================================

    MasterNodeElements & m = masterNodeElems;
    std::vector<CommNode*>  & ag = agentsNodes;
    std::string & thisHost = currentHostAddr;
    int & startingPort = initialPort;

    masterAddress = cfg.network.masterNode();
    isMasterHost = (thisHost == masterAddress);

    TRC("MasterAddress is " << masterAddress);

    //======================================================================
    // 2. Create the elements and connections for the host we are running on
    //======================================================================

    // Agent names, hosts and ports (port range starts with startingPort)
    char sAgName[20];
    std::vector<std::string> agName;
    std::vector<std::string> agHost;
    std::vector<int> agPortTsk;

    // Connection addresses and channel
    std::string bindAddr;
    std::string connAddr;
    std::string chnl;

    //=== If we are running on a processing host ==========================
    if (! isMasterHost) {

        //-----------------------------------------------------------------
        // a. Fill agents vector with as agents for this host
        //-----------------------------------------------------------------

        int h = 1;
        for (auto & kv : cfg.network.processingNodes()) {
            if (thisHost == kv.first) {
                int numOfTskAgents = kv.second;
                for (unsigned int i = 0; i < numOfTskAgents; ++i) {
                    sprintf(sAgName, "TskAgent_%02d_%02d", h, i + 1);
                    cfg.agentNames.push_back(sAgName);
                    TskAge * tskag = new TskAge(sAgName, thisHost, &synchro);
                    // By default, task agents are assumed to live in remote hosts
                    tskag->setRemote(true);
                    tskag->setSysDir(Config::PATHRun);
                    tskag->setWorkDir(Config::PATHTsk);
                    ag.push_back(tskag);
                    agName.push_back(std::string(sAgName));
                    agPortTsk.push_back(portnum(startingPort + 1, h, i));
                }
            }
            ++h;
        }

        //-----------------------------------------------------------------
        // b. Create Swarm Manager if serviceNodes is not empty and the
        //    current host is the first in the list (Swam Manager)
        //-----------------------------------------------------------------

        if (cfg.network.serviceNodes().size() > 0) {
            if (thisHost == cfg.network.serviceNodes().at(0)) {
                sprintf(sAgName, "TskAgentSwarm");
                ag.push_back(new TskAge(sAgName, thisHost, &synchro, TskAge::SERVICE));
                cfg.agentNames.push_back(sAgName);
                agName.push_back(std::string(sAgName));
                agPortTsk.push_back(portnum(startingPort + 1, h, 0));
            }
        }

        //-----------------------------------------------------------------
        // c. Create agent connections
        //-----------------------------------------------------------------

        // CHANNEL TASK-PROCESSING - REQREP
        // - Out/In: TskAge*/TskMng
        // Note that this channel is used to send from TskAgents to TskManager:
        // 1. Processing requests
        // 2. Processing status reports
        // 3. Processing completion messages
        h = 0;
        for (auto & a : ag) {
            chnl = ChnlTskProc + "_" + agName.at(h);
            TRC("### Connections for channel " << chnl);
            connAddr = "tcp://" + masterAddress + ":" + str::toStr<int>(agPortTsk.at(h));
            ReqRep * r = new ReqRep(NN_REQ, connAddr);
            a->addConnection(chnl, r);
            ++h;
        }

        return;
    }

    //=== Else, we are running on the master host =========================

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
            agPortTsk.push_back(portnum(startingPort + 1, h, i));
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
    TRC("### Connections for channel " << chnl);
    bindAddr = "inproc://" + chnl;
    connAddr = bindAddr;
    Survey * surveyor = new Survey(NN_SURVEYOR, bindAddr);
    surveyor->setNumOfRespondents(4);
    m.evtMng->addConnection(chnl, surveyor);
    for (auto & c : std::vector<CommNode*> {m.datMng, m.logMng, m.tskOrc, m.tskMng}) {
        c->addConnection(chnl, new Survey(NN_RESPONDENT, connAddr));
    }

    // CHANNEL HMICMD - REQREP
    // - Out/In: QPFHMI/EvtMng
    chnl     = ChnlHMICmd;
    TRC("### Connections for channel " << chnl);
    bindAddr = "tcp://" + masterAddress + ":" + str::toStr<int>(startingPort);
    m.evtMng->addConnection(chnl, new ReqRep(NN_REP, bindAddr));

    // CHANNEL INDATA -  PUBSUB
    // - Publisher: EvtMng
    // - Subscriber: DataMng TskOrc
    chnl     = ChnlInData;
    TRC("### Connections for channel " << chnl);
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
    TRC("### Connections for channel " << chnl);
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
        TRC("### Connections for channel " << chnl);
        bindAddr = "tcp://" + masterAddress + ":" + str::toStr<int>(p);
        m.tskMng->addConnection(chnl, new ReqRep(NN_REP, bindAddr));
        ++h;
    }

    // CHANNEL TASK-REPORTING-DISTRIBUTION - PUBSUB
    // - Publisher: TskMng
    // - Subscriber: DataMng EvtMng QPFHMI
    chnl     = ChnlTskRepDist;
    TRC("### Connections for channel " << chnl);
    bindAddr = "inproc://" + chnl;
    connAddr = bindAddr;
    m.tskMng->addConnection(chnl, new PubSub(NN_PUB, bindAddr));
    for (auto & c: std::vector<CommNode*> {m.datMng, m.evtMng}) {
        c->addConnection(chnl, new PubSub(NN_SUB, connAddr));
    }

}

//}
