/******************************************************************************
 * File:    datamng.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.DataManager
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
 *   Declare DataManager class
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

#ifndef DATAMNG_H
#define DATAMNG_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - component.h
//------------------------------------------------------------
#include "component.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

class DataMng : public Component {
public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    DataMng(const char * name, const char * addr = 0, Synchronizer * s = 0);

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    DataMng(std::string name, std::string addr = std::string(), Synchronizer * s = 0);

    //----------------------------------------------------------------------
    // Method: fromRunningToOperational
    //----------------------------------------------------------------------
    virtual void fromRunningToOperational();

    //----------------------------------------------------------------------
    // Method: fromOperationalToRunning
    //----------------------------------------------------------------------
    virtual void fromOperationalToRunning();

    //----------------------------------------------------------------------
    // Method: fromRunningToOff
    //----------------------------------------------------------------------
    virtual void fromRunningToOff();

    //----------------------------------------------------------------------
    // Method: runEachIteration
    //----------------------------------------------------------------------
    virtual void runEachIteration();

protected:
    //----------------------------------------------------------------------
    // Method: processCmdMsg
    //----------------------------------------------------------------------
    virtual void processCmdMsg(ScalabilityProtocolRole * conn, MessageString & m);

    //----------------------------------------------------------------------
    // Method: processInDataMsg
    //----------------------------------------------------------------------
    virtual void processInDataMsg(ScalabilityProtocolRole * conn, MessageString & m);

    //----------------------------------------------------------------------
    // Method: processTskSchedMsg
    //----------------------------------------------------------------------
    virtual void processTskSchedMsg(ScalabilityProtocolRole * conn, MessageString & m);

 protected:

    //----------------------------------------------------------------------
    // Method: initializeDB
    // Initialize the DB
    // (currently just an INI text file)
    //----------------------------------------------------------------------
    void initializeDB();

    //----------------------------------------------------------------------
    // Method: saveToDB
    // Save the information of a new (incoming) product to the DB
    // (currently just an INI text file)
    //----------------------------------------------------------------------
    void saveToDB(Message_INDATA * msg);

    //----------------------------------------------------------------------
    // Method: saveTaskToDB
    // Save the information on generated output products to the archive
    //----------------------------------------------------------------------
    void saveTaskToDB(Message_TASK_Processing * msg, bool initialStore = false);

    //----------------------------------------------------------------------
    // Method: sanitizeProductVersions
    // Make sure that there is no product with the same signature (and version)
    // in local archive, changing the version if needed
    //----------------------------------------------------------------------
    void sanitizeProductVersions(ProductCollection & productList);

    //----------------------------------------------------------------------
    // Method: saveProductsToDB
    // Save the information of a new (incoming) product to the DB
    // (currently just an INI text file)
    //----------------------------------------------------------------------
    void saveProductsToDB(ProductCollection & productList); //Json::Value & prodMetadata);

    //----------------------------------------------------------------------
    // Method: archiveDSSnEAS
    // Sends the information to the area where the corresponding daemon is
    // looking for data to be sent to DSS/EAS
    //----------------------------------------------------------------------
    void archiveDSSnEAS(ProductCollection & productList);

private:
    std::string dbFileName;
};

}

#endif
