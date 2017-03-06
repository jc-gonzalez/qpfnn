/******************************************************************************
 * File:    dbhdl.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.DBHandler
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
 *   Declare MessageHandler class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   MsgTypes
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

#ifndef DBHDL_H
#define DBHDL_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//   - string
//   - map
//   - vector
//   - utility
//------------------------------------------------------------
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <utility>

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - propdef.h
//   - msghdl.h
//------------------------------------------------------------
#include "propdef.h"
#include "datatypes.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: DBHandler
//==========================================================================
class DBHandler {

public:

    //----------------------------------------------------------------------
    // Constructor: DBHdlPostgreSQL
    //----------------------------------------------------------------------
    DBHandler() : connectionParamsSet(false) {}

    //----------------------------------------------------------------------
    // Destructor: ~DBHdlPostgreSQL (virtual)
    //----------------------------------------------------------------------
    virtual ~DBHandler() {}

    Property(DBHandler, std::string, dbHost, DbHost);
    Property(DBHandler, std::string, dbPort, DbPort);
    Property(DBHandler, std::string, dbName, DbName);
    Property(DBHandler, std::string, dbUser, DbUser);
    Property(DBHandler, std::string, dbPasswd, DbPasswd);
    Property(DBHandler, std::string, dbConnParams, DbConnectionParams);

public:

    //----------------------------------------------------------------------
    // Method: openConnection
    // Open a connection to the database
    //----------------------------------------------------------------------
    virtual bool openConnection(const char * data = 0)=0;

    //----------------------------------------------------------------------
    // Method: closeConnection
    // Closes the connection to the database
    //-------------------------------DBHdlPostgreSQL::---------------------------------------
    virtual bool closeConnection(const char * data = 0)=0;

    //----------------------------------------------------------------------
    // Method: storeProducts
    // Saves a set of products' metadata to the database
    //----------------------------------------------------------------------
    virtual int storeProducts(ProductCollection & prodList)=0;

    //----------------------------------------------------------------------
    // Method: retrieveProducts
    // Retrieves a set of products from the database, according to
    // pre-defined criteria
    //----------------------------------------------------------------------
    virtual int retrieveProducts(ProductList & prodList,
                                 std::string criteria = std::string())=0;

    //----------------------------------------------------------------------
    // Method: storeTask
    // Stores a task information to the database
    //----------------------------------------------------------------------
    virtual bool storeTask(TaskInfo & task)=0;

    //----------------------------------------------------------------------
    // Method: checkTask
    // Returns true if an entry for a task exists in the DB
    //----------------------------------------------------------------------
    virtual bool checkTask(std::string taskId = std::string())=0;

    //----------------------------------------------------------------------
    // Method: updateTask
    // Updates the information for a given task in the database
    //----------------------------------------------------------------------
    virtual bool updateTask(TaskInfo & task)=0;

    //----------------------------------------------------------------------
    // Method: retrieveTask
    // Retrieves a task information from the database
    //----------------------------------------------------------------------
    virtual bool retrieveTask(TaskInfo & task)=0;

    //----------------------------------------------------------------------
    // Method: storeState
    // Stores a new state to the database, for a given node and session
    //----------------------------------------------------------------------
    virtual bool storeState(std::string session, std::string node, std::string newState)=0;

    //----------------------------------------------------------------------
    // Method: getCurrentState
    // Gets the list of nodes with its states for a given session
    //----------------------------------------------------------------------
    virtual std::vector< std::vector<std::string> > getCurrentState(std::string session)=0;


    //----------------------------------------------------------------------
    // Method: getLatestState
    // Gets the last registered session name and state of the Event Manager
    //----------------------------------------------------------------------
    virtual std::pair<std::string, std::string> getLatestState()=0;

    //----------------------------------------------------------------------
    // Method: getICommand
    // Gets a new command into the icommands table
    //----------------------------------------------------------------------
    virtual void addICommand(std::string target,
                             std::string source,
                             std::string content)=0;

    //----------------------------------------------------------------------
    // Method: getICommand
    // Gets a command from the icommands table
    //----------------------------------------------------------------------
    virtual bool getICommand(std::string target,
                             int & id,
                             std::string & source,
                             std::string & content)=0;

    //----------------------------------------------------------------------
    // Method: markICommandAsDone
    // Sets the icommands' executed flag to true
    //----------------------------------------------------------------------
    virtual bool markICommandAsDone(int id)=0;

    //----------------------------------------------------------------------
    // Method: removeICommand
    // Remove the command using its id
    //----------------------------------------------------------------------
    virtual bool removeICommand(int id)=0;

    //----------------------------------------------------------------------
    // Method: storeMsg
    // Stores a message into the database
    //----------------------------------------------------------------------
    virtual bool storeMsg(std::string from,
                          Router2RouterPeer::PeerMessage & inPeerMsg,
                          bool isBroadcast)=0;

    //----------------------------------------------------------------------
    // Method: retrieveMsgs
    // Retrieves a set of messages from the database, according to
    // pre-defined criteria
    //----------------------------------------------------------------------
    virtual bool retrieveMsgs(std::vector<std::pair<std::string,
                              Router2RouterPeer::PeerMessage> > & msgList,
                              std::string criteria = std::string())=0;

    //----------------------------------------------------------------------
    // Method: getTable
    // Get the content (all the records) from a given table
    //----------------------------------------------------------------------
    virtual bool getTable(std::string tName,
                          std::vector< std::vector<std::string> > & table)=0;

    //----------------------------------------------------------------------
    // Method: getQuery
    // Get the result (all the records) of a given query
    //----------------------------------------------------------------------
    virtual bool getQuery(std::string qry,
                          std::vector< std::vector<std::string> > & table)=0;

    //----------------------------------------------------------------------
    // Method: getNumRowsInTable
    // Get the content (all the records) from a given table
    //----------------------------------------------------------------------
    virtual int getNumRowsInTable(std::string tName)=0;

    //----------------------------------------------------------------------
    // Method: runCmd
    // Runs a SQL command
    //----------------------------------------------------------------------
    virtual bool runCmd(std::string cmd)=0;

    //----------------------------------------------------------------------
    // Method: fillWithResult
    // Retrieves the content of the returned n-tuples (after and SQL command)
    // into the table parameter
    //----------------------------------------------------------------------
    virtual bool fillWithResult(std::vector< std::vector<std::string> > & table)=0;

    //----------------------------------------------------------------------
    // Method: getVersionCounter
    // Returns the process version counter for a given processor
    //----------------------------------------------------------------------
    virtual int getVersionCounter(std::string & procName)=0;

    //----------------------------------------------------------------------
    // Method: checkSignature
    // Check if a product with the same signature exists in the archive
    //----------------------------------------------------------------------
    virtual bool checkSignature(std::string & sgnt, std::string & ver)=0;

    //----------------------------------------------------------------------
    // Method: updateTable<T>
    // Template method to update a single field of a table
    //----------------------------------------------------------------------
    template<class T>
    bool updateTable(std::string table, std::string cond,
                             std::string field, T value);

protected:
    bool connectionParamsSet;

};

//}

#endif  /* DBHDL_H */
