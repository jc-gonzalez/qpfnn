/******************************************************************************
 * File:    cntrmng.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.ContainerMng
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
 *   Declare ContainerMng class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   Component
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

#ifndef CNTRMNG_H
#define CNTRMNG_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   none
//------------------------------------------------------------
#include <vector>
#include <string>
#include <sstream>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   none
//------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: ContainerMng
//==========================================================================
class ContainerMng {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    ContainerMng(std::string mngAddr, std::vector<std::string> wrkAddrs);

    //----------------------------------------------------------------------
    // Method: initSwarmManager
    // Initializes the swarm manager
    //----------------------------------------------------------------------
    bool initSwarmManager(std::string & addr);

    //----------------------------------------------------------------------
    // Method: initSwarmWorker
    // Initializes a swarm worker
    //----------------------------------------------------------------------
    bool initSwarmWorker(std::string & addr);

    //----------------------------------------------------------------------
    // Method: createService
    // Creates a service that retrieves data from TskMng & processes them
    //----------------------------------------------------------------------
    bool createService(std::string srv, std::string img,
                       std::string exe, std::vector<std::string> args,
                       int numScale);

    //----------------------------------------------------------------------
    // Method: reScaleService
    // Rescales a running service
    //----------------------------------------------------------------------
    bool reScaleService(std::string srv, int newScale);

    //----------------------------------------------------------------------
    // Method: getServiceInfo
    // Retrieves information about running service
    //----------------------------------------------------------------------
    bool getServiceInfo(std::string srv, std::stringstream & info);

    //----------------------------------------------------------------------
    // Method: shutdownService
    // Retrieves information about running service
    //----------------------------------------------------------------------
    bool shutdownService(std::string srv);

    //----------------------------------------------------------------------
    // Method: leaveSwarm
    // Make a node leave the swarm
    //----------------------------------------------------------------------
    bool leaveSwarm(std::string & addr);

    //----------------------------------------------------------------------
    // Method: shutdownSwarm
    //Shutdown entire swarm
    //----------------------------------------------------------------------
    bool shutdownSwarm(std::string srv);

private:
    std::string managerAddr;
    std::vector<std::string> workerAddrs;
    std::string workerToken;
};

//}

#endif  /* TASKAGENT_H */
