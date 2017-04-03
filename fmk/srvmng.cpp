/******************************************************************************
 * File:    srvmng.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.srvmng
 *
 * Version:  1.1
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement ServiceMng class
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

#include "srvmng.h"

#include "process.h"
#include "str.h"

#include <cassert>

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
ServiceMng::ServiceMng(std::string mngAddr, std::vector<std::string> wrkAddrs)
{
    managerAddr = mngAddr;
    workerAddrs = wrkAddrs;

    // Initialize Swarm manager and workers
    assert(initSwarmManager(managerAddr));
    for (auto & wrkAddr : workerAddrs) { assert(initSwarmWorker(wrkAddr)); }
}

//----------------------------------------------------------------------
// Method: initSwarmManager
// Initializes the swarm manager
//----------------------------------------------------------------------
bool ServiceMng::initSwarmManager(std::string & addr)
{
    procxx::process initSwarm("docker", "swarm", "init", "--advertise-addr", addr);
    initSwarm.exec();
    initSwarm.wait();
    return (initSwarm.code() == 0);
}

//----------------------------------------------------------------------
// Method: initSwarmWorker
// Initializes a swarm worker
//----------------------------------------------------------------------
bool ServiceMng::initSwarmWorker(std::string & addr)
{
    procxx::process initWorker("ssh", "-Y", "-C", "-l", "eucops", addr,
                               "docker", "swarm", "init",
                               "--advertise-addr", addr);
    initWorker.exec();
    initWorker.wait();
    return (initWorker.code() == 0);
}

//----------------------------------------------------------------------
// Method: createService
// Creates a service that retrieves data from TskMng & processes them
//----------------------------------------------------------------------
bool ServiceMng::createService(std::string srv, std::string img, int numScale,
                   std::string exe, std::vector<std::string> args)
{
    procxx::process srvCreate("docker", "service", "create",
                              "--replicas", str::toStr<int>(numScale),
                              exe);
    for (auto & a : args) { srvCreate.add_argument(a); }
    srvCreate.exec();
    srvCreate.wait();
    return (srvCreate.code() == 0);
}

//----------------------------------------------------------------------
// Method: reScaleService
// Rescales a running service
//----------------------------------------------------------------------
bool ServiceMng::reScaleService(std::string srv, int newScale)
{
    procxx::process srvScale("docker", "service", "scale",
                              srv + "=" + str::toStr<int>(newScale));
    srvScale.exec();
    srvScale.wait();
    return (srvScale.code() == 0);
}

//----------------------------------------------------------------------
// Method: getServiceInfo
// Retrieves information about running service
//----------------------------------------------------------------------
bool ServiceMng::getServiceInfo(std::string srv, std::stringstream & info)
{
    procxx::process srvInspect("sdocker", "service", "inspect");
    srvInspect.add_argument(srv);
    srvInspect.exec();

    info.str("");
    std::string line;
    while (std::getline(srvInspect.output(), line)) {
        info << line << std::endl;
        if (!srvInspect.running() ||
            !procxx::running(srvInspect.id()) ||
            !running(srvInspect)) {
            break;
        }
    }

    srvInspect.wait();
    return (srvInspect.code() == 0);
}

//----------------------------------------------------------------------
// Method: shutdownService
// Shutdown a given service
//----------------------------------------------------------------------
bool ServiceMng::shutdownService(std::string srv)
{
    procxx::process srvRm("docker", "service", "rm");
    srvRm.add_argument(srv);
    srvRm.exec();
    srvRm.wait();
    return (srvRm.code() == 0);
}

//----------------------------------------------------------------------
// Method: leaveSwarm
// Make a node leave the swarm
//----------------------------------------------------------------------
bool ServiceMng::leaveSwarm(std::string & addr)
{
    if (addr == managerAddr) {
        procxx::process swrmLeave("docker", "swarm", "leave");
        swrmLeave.exec();
        swrmLeave.wait();
        return (swrmLeave.code() == 0);
    } else {
        procxx::process swrmLeave("ssh", "-Y", "-C", "-l", "eucops", addr,
                                  "docker", "swarm", "leave");
        swrmLeave.exec();
        swrmLeave.wait();
        return (swrmLeave.code() == 0);
    }
}

//----------------------------------------------------------------------
// Method: shutdownSwarm
//Shutdown entire swarm
//----------------------------------------------------------------------
bool ServiceMng::shutdownSwarm(std::string srv)
{
    // Initialize Swarm manager and workers
    assert(leaveSwarm(managerAddr));
    for (auto & wrkAddr : workerAddrs) { assert(leaveSwarm(wrkAddr)); }
    return true;
}

//}
