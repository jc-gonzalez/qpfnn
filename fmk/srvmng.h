/******************************************************************************
 * File:    srvmng.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.ServiceMng
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
 *   Declare ServiceMng class
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

#ifndef SRVMNG_H
#define SRVMNG_H

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
#include "dckmng.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: ServiceMng
//==========================================================================
class ServiceMng : public DockerMng {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    ServiceMng(std::string mngAddr, std::vector<std::string> wrkAddrs);

    //----------------------------------------------------------------------
    // Method: createService
    // Creates a service that retrieves data from TskMng & processes them
    //----------------------------------------------------------------------
    virtual bool createService(std::string srv, std::string img, int numScale,
                               std::string exe, std::vector<std::string> args);

    //----------------------------------------------------------------------
    // Method: reScaleService
    // Rescales a running service
    //----------------------------------------------------------------------
    virtual bool reScaleService(std::string id, int newScale);

    //----------------------------------------------------------------------
    // Method: getInfo
    // Retrieves information about running service
    //----------------------------------------------------------------------
    virtual bool getInfo(std::string id, std::stringstream & info);

    //----------------------------------------------------------------------
    // Method: kill
    // Retrieves information about running service
    //----------------------------------------------------------------------
    virtual bool kill(std::string id);

    //----------------------------------------------------------------------
    // Method: leaveSwarm
    // Make a node leave the swarm
    //----------------------------------------------------------------------
    virtual bool leaveSwarm(std::string & addr);

    //----------------------------------------------------------------------
    // Method: shutdown
    //Shutdown entire swarm
    //----------------------------------------------------------------------
    virtual bool shutdown(std::string srv);

private:
    //----------------------------------------------------------------------
    // Method: initSwarmManager
    // Initializes the swarm manager
    //----------------------------------------------------------------------
    virtual bool initSwarmManager(std::string & addr);

    //----------------------------------------------------------------------
    // Method: initSwarmWorker
    // Initializes a swarm worker
    //----------------------------------------------------------------------
    virtual bool initSwarmWorker(std::string & addr);

private:
    std::string managerAddr;
    std::vector<std::string> workerAddrs;
    std::string workerToken;
};

//}

#endif  /* SRVMNG_H */
