/******************************************************************************
 * File:    deployer.h
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
 *   Declare Deployer class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   CommNode
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

#ifndef DEPLOYER_H
#define DEPLOYER_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - map
//   - vector
//   - fstream
//------------------------------------------------------------
#include <vector>
#include <map>
#include <csignal>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   none
//------------------------------------------------------------
#include "evtmng.h"
#include "datamng.h"
#include "logmng.h"
#include "tskorc.h"
#include "tskmng.h"
#include "tskage.h"

#include "version.h"

#ifndef BUILD_ID
#define BUILD_ID ""
#endif

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: Deployer
//==========================================================================
class Deployer {

public:
    //----------------------------------------------------------------------
    // Constructor: Deployer
    //----------------------------------------------------------------------
    Deployer(int argc, char *argv[]);

    //----------------------------------------------------------------------
    // Destructor: Deployer
    //----------------------------------------------------------------------
    ~Deployer();

    //----------------------------------------------------------------------
    // Method: run
    // Launches the system components and starts the system
    //----------------------------------------------------------------------
    int run();

    //----------------------------------------------------------------------
    // Method: actionOnSigInt
    // Actions to be performed when capturing SigInt
    //----------------------------------------------------------------------
    void actionOnSigInt();

private:
    //----------------------------------------------------------------------
    // Method: usage
    // Shows usage information
    //----------------------------------------------------------------------
    bool usage(int code);

    //----------------------------------------------------------------------
    // Method: processCmdLineOpts
    // Processes command line options to configure execution
    //----------------------------------------------------------------------
    bool processCmdLineOpts(int argc, char * argv[]);

    //----------------------------------------------------------------------
    // Method: setConfigFile
    // Sets the name of the configuration file to be used
    //----------------------------------------------------------------------
    void setConfigFile(std::string fName);

    //----------------------------------------------------------------------
    // Method: setInitialPort
    // Sets the initial port for communications set up
    //----------------------------------------------------------------------
    void setInitialPort(int port);

    //----------------------------------------------------------------------
    // Method: setCurrentHostName
    // Set the host name the current host
    //----------------------------------------------------------------------
    void setCurrentHostName(std::string host);

    //----------------------------------------------------------------------
    // Method: setCurrentHostAddr
    // Set the address (IP) of the current host
    //----------------------------------------------------------------------
    void setCurrentHostAddr(std::string addr);

    //----------------------------------------------------------------------
    // Method: readConfiguration
    // Retrieves the configuration for the execution of the system (from
    // a disk file or from the internal database);
    //----------------------------------------------------------------------
    void readConfiguration();

    //----------------------------------------------------------------------
    // Method: delay
    // Waits for a small time lapse for system sync
    //----------------------------------------------------------------------
    int delay(int ms);

    //----------------------------------------------------------------------
    // Method: sayHello
    // Shows a minimal title and build id for the application
    //----------------------------------------------------------------------
    void sayHello();

    //----------------------------------------------------------------------
    // Method: getHostnameAndIp
    // Gets from the system the host name and ip address of the host
    //----------------------------------------------------------------------
    void getHostnameAndIp(std::string & hName, std::string & ipAddr);

    //----------------------------------------------------------------------
    // Method: existsDir
    // Removes old log and msg files
    //----------------------------------------------------------------------
    bool existsDir(std::string pathName);

    //----------------------------------------------------------------------
    // Method: installSignalHandlers
    // Install signal handlers
    //----------------------------------------------------------------------
    void installSignalHandlers();

    //----------------------------------------------------------------------
    // createElementsNetwork
    //----------------------------------------------------------------------
    void createElementsNetwork();

    //----------------------------------------------------------------------
    // generateProcFmkInfoStructure
    //----------------------------------------------------------------------
    void generateProcFmkInfoStructure();

private:
    //----------------------------------------------------------------------
    // Variable: sigIntHandler
    // Standard structure for signal handling
    //----------------------------------------------------------------------
    struct sigaction sigIntHandler;

private:
    struct MasterNodeElements {
        EvtMng  * evtMng;
        DataMng * datMng;
        LogMng  * logMng;
        TskOrc  * tskOrc;
        TskMng  * tskMng;
    };

    typedef MasterNodeElements * MasterNodeElementsPtr;

    MasterNodeElements     masterNodeElems;
    std::vector<CommNode*> agentsNodes;

    Synchronizer           synchro;

    std::string            cfgFileName;
    std::string            currentHostAddr;
    std::string            currentHostName;
    int                    initialPort;

    std::string            exeName;

    std::string            masterAddress;
    bool                   isMasterHost;

};

//}

#endif  /* DEPLOYER_H */
