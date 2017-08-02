/******************************************************************************
 * File:    dbhdlpostgre.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.DBHdlPostgreSQL
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
 *   Declare MessageHandler class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   DBHandler
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

#ifndef DBHDLPOSTGRE_H
#define DBHDLPOSTGRE_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: External packages
//   - libpq-fe.h
//------------------------------------------------------------
#include <libpq-fe.h>

//------------------------------------------------------------
// Topic: Project headers
//   - dbhdl.h
//------------------------------------------------------------
#include "dbhdl.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: DBHdlPostgreSQL
//==========================================================================
class DBHdlPostgreSQL : public DBHandler {

public:

    //----------------------------------------------------------------------
    // Constructor: DBHdlPostgreSQL
    //----------------------------------------------------------------------
    DBHdlPostgreSQL();

    //----------------------------------------------------------------------
    // Destructor: ~DBHdlPostgreSQL (virtual)
    //----------------------------------------------------------------------
    ~DBHdlPostgreSQL() {}

    //----------------------------------------------------------------------
    // Method: openConnection
    // Open a connection to the database
    //----------------------------------------------------------------------
    virtual bool openConnection(const char * data = 0);

    //----------------------------------------------------------------------
    // Method: closeConnection
    // Closes the connection to the database
    //----------------------------------------------------------------------
    virtual bool closeConnection(const char * data = 0);

    //----------------------------------------------------------------------
    // Method: storeProducts
    // Saves a set of products' metadata to the database
    //----------------------------------------------------------------------
    virtual int storeProducts(ProductList & prodList);

    //----------------------------------------------------------------------
    // Method: retrieveProducts
    // Retrieves a set of products from the database, according to
    // pre-defined criteria
    //----------------------------------------------------------------------
    virtual int retrieveProducts(ProductList & prodList,
                                 std::string criteria = std::string());

    //----------------------------------------------------------------------
    // Method: storeTask
    // Stores a task information to the database
    //----------------------------------------------------------------------
    virtual bool storeTask(TaskInfo & task);

    //----------------------------------------------------------------------
    // Method: checkTask
    // Returns true if an entry for a task exists in the DB
    //----------------------------------------------------------------------
    virtual bool checkTask(std::string taskId = std::string());

    //----------------------------------------------------------------------
    // Method: updateTask
    // Updates the information for a given task in the database
    //----------------------------------------------------------------------
    virtual bool updateTask(TaskInfo & task);

    //----------------------------------------------------------------------
    // Method: retrieveTask
    // Retrieves a task information from the database
    //----------------------------------------------------------------------
    virtual bool retrieveTask(TaskInfo & task);

    //----------------------------------------------------------------------
    // Method: storeState
    // Stores a new state to the database, for a given node and session
    //----------------------------------------------------------------------
    virtual bool storeState(std::string session, std::string node, std::string newState);

    //----------------------------------------------------------------------
    // Method: getCurrentState
    // Gets the list of nodes with its states for a given session
    //----------------------------------------------------------------------
    virtual std::vector< std::vector<std::string> > getCurrentState(std::string session);

    //----------------------------------------------------------------------
    // Method: getLatestState
    // Gets the last registered session name and state of the Event Manager
    //----------------------------------------------------------------------
    virtual std::pair<std::string, std::string> getLatestState();

    //----------------------------------------------------------------------
    // Method: getICommand
    // Gets a new command into the icommands table
    //----------------------------------------------------------------------
    virtual void addICommand(std::string target,
                             std::string source,
                             std::string content);

    //----------------------------------------------------------------------
    // Method: getICommand
    // Gets a command from the icommands table
    //----------------------------------------------------------------------
    virtual bool getICommand(std::string target,
                             int & id,
                             std::string & source,
                             std::string & content);

    //----------------------------------------------------------------------
    // Method: markICommandAsDone
    // Sets the icommands' executed flag to true
    //----------------------------------------------------------------------
    virtual bool markICommandAsDone(int id);

    //----------------------------------------------------------------------
    // Method: removeICommand
    // Remove the command using its id
    //----------------------------------------------------------------------
    virtual bool removeICommand(int id);

    //----------------------------------------------------------------------
    // Method: storeMsg
    // Stores a message into the database
    //----------------------------------------------------------------------
    virtual bool storeMsg(std::string from,
                          MessageString & inPeerMsg,
                          bool isBroadcast);

    //----------------------------------------------------------------------
    // Method: retrieveMsgs
    // Retrieves a set of messages from the database, according to
    // pre-defined criteria
    //----------------------------------------------------------------------
    virtual bool retrieveMsgs(std::vector<std::pair<std::string,
                              MessageString> > & msgList,
                              std::string criteria = std::string());

    //----------------------------------------------------------------------
    // Method: getTable
    // Get the content (all the records) from a given table
    //----------------------------------------------------------------------
    virtual bool getTable(std::string tName,
                          std::vector< std::vector<std::string> > & table);

    //----------------------------------------------------------------------
    // Method: getQuery
    // Get the result (all the records) of a given query
    //----------------------------------------------------------------------
    virtual bool getQuery(std::string qry,
                          std::vector< std::vector<std::string> > & table);

    //----------------------------------------------------------------------
    // Method: getNumRowsInTable
    // Get the content (all the records) from a given table
    //----------------------------------------------------------------------
    virtual int getNumRowsInTable(std::string tName);

    //----------------------------------------------------------------------
    // Method: runCmd
    // Runs a SQL command
    //----------------------------------------------------------------------
    virtual bool runCmd(std::string cmd);

    //----------------------------------------------------------------------
    // Method: fillWithResult
    // Retrieves the content of the returned n-tuples (after and SQL command)
    // into the table parameter
    //----------------------------------------------------------------------
    virtual bool fillWithResult(std::vector< std::vector<std::string> > & table);

    //----------------------------------------------------------------------
    // Method: getVersionCounter
    // Returns the process version counter for a given processor
    //----------------------------------------------------------------------
    virtual int getVersionCounter(std::string & procName);

    //----------------------------------------------------------------------
    // Method: checkSignature
    // Check if a product with the same signature exists in the archive
    //----------------------------------------------------------------------
    virtual bool checkSignature(std::string & sgnt, std::string & ver);

private:

    //----------------------------------------------------------------------
    // Method: eqKeyValue
    // Returns the value used in the update string
    //----------------------------------------------------------------------
    std::string eqKeyValue(std::string k, int x);
    std::string eqKeyValue(std::string k, double x);
    std::string eqKeyValue(std::string k, std::string x);
    std::string eqKeyValue(std::string k, Json::Value x);

    //----------------------------------------------------------------------
    // Method: updateTable
    // Method to update a series of fields of a table
    //----------------------------------------------------------------------
    bool updateTable(std::string table, std::string cond,
                     std::vector<std::string> & newValues);

private:
    PGconn     * conn;
    PGresult   * res;
};

//}

#endif  /* DBHDLPOSTGRE_H */
