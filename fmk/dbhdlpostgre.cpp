/******************************************************************************
 * File:    dbhdlpostgre.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.dbhdlpostgre
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
 *   Implement MessageHandler class
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

#include <iterator>
#include <arpa/inet.h>

#include "dbhdlpostgre.h"

#include "except.h"
#include "tools.h"
#include "json/json.h"
#include "config.h"
#include "dbg.h"
#include "str.h"
#include "message.h"

using Configuration::cfg;

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//----------------------------------------------------------------------
// Constructor: DBHdlPostgreSQL
//----------------------------------------------------------------------
DBHdlPostgreSQL::DBHdlPostgreSQL() : conn(0)
{
}

//----------------------------------------------------------------------
// Method: openConnection
// Open a connection to the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::openConnection(const char * data)
{
    // Generate new connection to database
    const char * connStr = data;
    if (data == 0) {
        if (!connectionParamsSet) {
            setDbHost(Config::DBHost);
            setDbPort(Config::DBPort);
            setDbName(Config::DBName);
            setDbUser(Config::DBUser);
            setDbPasswd(Config::DBPwd);

            setDbConnectionParams("host=" + getDbHost() +
                                  " port=" + getDbPort() +
                                  " dbname=" + getDbName() +
                                  " user=" + getDbUser() +
                                  " password=" + getDbPasswd());
            connectionParamsSet = true;
        }
        connStr = getDbConnectionParams().c_str();
    }
    if (conn != 0) {
        char thid[10];
        sprintf(thid, "%ld", pthread_self());
        std::string msg = (std::string("Connection will be re-open at thread ") +
                           std::string(thid));
        PQfinish(conn);
        throw RuntimeException(msg);
    }
    conn = PQconnectdb(connStr);
    if (PQstatus(conn) != CONNECTION_OK) {
        std::string msg = (std::string("Connection to database failed: ") +
                           std::string(PQerrorMessage(conn)));
        PQfinish(conn);
        throw RuntimeException(msg);
    }
    return true;
}

//----------------------------------------------------------------------
// Method: closeConnection
// Closes the connection to the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::closeConnection(const char * data)
{
    UNUSED(data);

    PQfinish(conn);
    return true;
}

//----------------------------------------------------------------------
// Method: storeProducts
// Saves a set of products' metadata to the database
//----------------------------------------------------------------------
int DBHdlPostgreSQL::storeProducts(ProductList & prodList)
{
    bool result;
    int nInsProd = 0;
    std::stringstream ss;

    for (auto & m : prodList.products) {
        ss.str("");
        TRC("Building SQL statement");
        TRC(m.startTime() << ", " << m.endTime() << ", " << timeTag());
        ss << "INSERT INTO products_info "
           << "(product_id, product_type, product_status_id, product_version, product_size, creator_id, "
           << "obs_id, soc_id, "
           << "instrument_id, obsmode_id, signature, start_time, end_time, registration_time, url) "
           << "VALUES ("
           << str::quoted(m.productId()) << ", "
           << str::quoted(m.productType()) << ", "
           << str::quoted("OK") << ", "
           << str::quoted(m.productVersion()) << ", "
           << m.productSize() << ", "
           << str::quoted("SOC_QLA_TEST") << ", "
           << str::quoted(m.obsIdStr()) << ", "
           << str::quoted(m.obsIdStr()) << ", "
           << str::quoted(m.instrument()) << ", "
           << str::quoted(m.obsMode()) << ", "
           << str::quoted(m.signature()) << ", "
           << str::quoted(str::tagToTimestamp(m.startTime())) << ", "
           << str::quoted(str::tagToTimestamp(m.endTime())) << ", "
           << str::quoted(str::tagToTimestamp(timeTag())) << ", "
           << str::quoted(m.url()) << ")";
        TRC(ss.str());
        try { result = runCmd(ss.str()); } catch(...) { throw; }
        TRC("Exeecuted.");
        PQclear(res);
        nInsProd++;
    }

    UNUSED(result);

    return nInsProd;
}

//----------------------------------------------------------------------
// Method: retrieveProducts
// Retrieves a set of products from the database, according to
// pre-defined criteria
//----------------------------------------------------------------------
int  DBHdlPostgreSQL::retrieveProducts(ProductList & prodList,
                                       std::string criteria)
{
    bool result;
    std::string cmd(
                "SELECT p.product_id, p.product_type, s.status_desc, p.product_version, "
                "p.product_size, c.creator_desc, i.instrument, m.obsmode_desc, "
                "p.start_time, p.end_time, p.registration_time, p.url "
                "FROM (((products_info AS p "
                "  INNER JOIN creators AS c "
                "  ON p.creator_id = c.creator_id) "
                "    INNER JOIN instruments AS i "
                "    ON p.instrument_id = i.instrument_id) "
                "      INNER JOIN observation_modes AS m "
                "      ON p.obsmode_id = m.obsmode_id) "
                "        INNER JOIN product_status AS s "
                "        ON p.product_status_id = s.product_status_id "
                "ORDER BY p.id ");
    cmd += criteria + ";";

    try { result = runCmd(cmd); } catch(...) { throw; }

    // Transfer the data to the product list argument
    //prodList.products.clear();
    json productArray;
    ProductMetadata m;
    int nRows = PQntuples(res);
    for (int i = 0; i < nRows; ++i) {
        m["productId"]      = std::string(PQgetvalue(res, i, 0));
        std::string prodType = std::string(PQgetvalue(res, i, 1));
        m["productType"]    = prodType;
        m["productStatus"]  = std::string(PQgetvalue(res, i, 2));
        m["productVersion"] = std::string(PQgetvalue(res, i, 3));
        m["productSize"]    =    *((int*)(PQgetvalue(res, i, 4)));
        m["creator"]        = std::string(PQgetvalue(res, i, 5));
        m["instrument"]     = std::string(PQgetvalue(res, i, 6));
        m["signature"]      = std::string(PQgetvalue(res, i, 7));
        m["startTime"]      = std::string(PQgetvalue(res, i, 8));
        m["endTime"]        = std::string(PQgetvalue(res, i, 9));
        m["regTime"]        = std::string(PQgetvalue(res, i, 10));
        m["url"]            = std::string(PQgetvalue(res, i, 11));
        productArray[i] = m.val();
        //prodList.products.push_back(m);
    }
    prodList = ProductList(productArray);
    PQclear(res);

    UNUSED(result);

    return nRows;
}

//----------------------------------------------------------------------
// Method: storeTask
// Stores a task information to the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::storeTask(TaskInfo & task)
{
    bool result = true;

    std::string registrationTime(tagToTimestamp(preciseTimeTag()));
    std::stringstream ss;
    std::string taskPath = task.taskPath();
    Json::StyledWriter writer;
    std::string taskData("{}");
    if (!task.taskData().empty()) {
        taskData = writer.write(task.taskData());
    }

    ss.str("");
    ss << "INSERT INTO tasks_info "
       << "(task_id, task_status_id, task_exitcode, task_path, "
       << "task_size, registration_time, task_data) "
       << "VALUES ("
       << str::quoted(task.taskName()) << ", "
       << task.taskStatus() << ", "
       << task.taskExitCode() << ", "
       << str::quoted(taskPath) << ", "
       << 0 << ", "
       << str::quoted(registrationTime) << ", "
       << str::quoted(taskData) << ")";
    TRC("PSQL> " << ss.str());
        
    try { result = runCmd(ss.str()); } catch(...) { throw; }

    PQclear(res);

    return result;
}

//----------------------------------------------------------------------
// Method: checkTask
// Returns true if an entry for a task exists in the DB
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::checkTask(std::string taskId)
{
    bool result;
    std::string cmd("SELECT t.task_id FROM tasks_info AS t "
                    "WHERE t.task_id=" + str::quoted(taskId) + ";");

    try { result = runCmd(cmd); } catch(...) { throw; }

    result &= (PQntuples(res) > 0);
    PQclear(res);

    return result;
}

//----------------------------------------------------------------------
// Method: updateTask
// Updates the information for a given task in the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::updateTask(TaskInfo & task)
{
    bool result = true;
    bool mustUpdate = true;

    std::string taskName = task.taskName().substr(15);
    json taskData  = task["taskData"];
    std::string id = taskData["Id"].asString();

    std::vector<std::string> updates {eqKeyValue("task_id", id)};
        
    if (checkTask(taskName)) {
        // Present, so this is task registered with old name as ID,
        // that must change to the actual ID
        result = updateTable("tasks_info",
                             eqKeyValue("task_id",taskName),
                             updates);
        // Once changed the task_id, the update must still be done
    }

    if (result) {
        updates.clear();
        updates.push_back(eqKeyValue("task_status_id", (int)(task.taskStatus())));
        updates.push_back(eqKeyValue("start_time", task.taskStart()));
        if (!task.taskEnd().empty()) {
            updates.push_back(eqKeyValue("end_time", task.taskEnd()));
        }
        updates.push_back(eqKeyValue("task_path", task.taskPath())); 
        updates.push_back(eqKeyValue("task_data", task["taskData"]));
        
        result &= updateTable("tasks_info",
                              eqKeyValue("task_id", id),
                              updates);
    }
     
    PQclear(res);
    return result;
}

//----------------------------------------------------------------------
// Method: retrieveTask
// Retrieves a task information from the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::retrieveTask(TaskInfo & task)
{
    UNUSED(task);

    return false;
}

//----------------------------------------------------------------------
// Method: storeState
// Stores a new state to the database, for a given node and session
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::storeState(std::string session, std::string node, std::string newState)
{
    bool result = true;

    std::string registrationTime(tagToTimestamp(preciseTimeTag()));
    std::string cmd("INSERT INTO qpfstates (timestmp, sessionname, nodename, state) VALUES (" +
                    str::quoted(registrationTime) + ", " +
                    str::quoted(session) + ", " +
                    str::quoted(node) + ", " +
                    str::quoted(newState) + ");");

    try { result = runCmd(cmd); } catch(...) { throw; }

    PQclear(res);

    return result;
}

//----------------------------------------------------------------------
// Method: getCurrentState
// Gets the list of nodes with its states for a given session
//----------------------------------------------------------------------
std::vector< std::vector<std::string> > DBHdlPostgreSQL::getCurrentState(std::string session)
{
    bool result = true;
    std::vector< std::vector<std::string> > table;

    std::string cmd("SELECT nodename, state FROM qpfstates "
                    "WHERE sessionname = " + str::quoted(session) +
                    "ORDER BY qpfstate_id;");
    try {
        result = runCmd(cmd);
        if (result) { result = fillWithResult(table); }
    } catch(...) {
        throw;
    }

    PQclear(res);
    return table;
}

//----------------------------------------------------------------------
// Method: getLatestState
// Gets the last registered session name and state of the Event Manager
//----------------------------------------------------------------------
std::pair<std::string, std::string> DBHdlPostgreSQL::getLatestState()
{
    bool result = true;
    std::pair<std::string, std::string> p;

    std::string cmd("SELECT sessionname, state FROM qpfstates "
                    "WHERE nodename = 'EvtMng' "
                    "ORDER BY qpfstate_id DESC LIMIT 1;");
    try {
        result = runCmd(cmd);
        if (result) {
            p.first   = std::string(PQgetvalue(res, 0, 0));
            p.second  = std::string(PQgetvalue(res, 0, 1));
        }
    } catch(...) {
        throw;
    }

    PQclear(res);
    return p;
}

//----------------------------------------------------------------------
// Method: getICommand
// Stores a new state to the database
//----------------------------------------------------------------------
void DBHdlPostgreSQL::addICommand(std::string target,
                                  std::string source,
                                  std::string content)
{
    bool result = true;

    std::string registrationTime(tagToTimestamp(preciseTimeTag()));
    std::string cmd("INSERT INTO icommands "
                    "(cmd_date, cmd_source, cmd_target, cmd_executed, cmd_content) "
                    "VALUES (" + str::quoted(registrationTime) +
                    ", " + str::quoted(source) +
                    ", " + str::quoted(target) +
                    ", false, " + str::quoted(content) + ");");
    try {
        result = runCmd(cmd);
    } catch(...) {
        throw;
    }

    PQclear(res);
    UNUSED(result);
}

//----------------------------------------------------------------------
// Method: getICommand
// Stores a new state to the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::getICommand(std::string target,
                                  int & id,
                                  std::string & source,
                                  std::string & content)
{
    bool result = true;

    std::string cmd("SELECT cmd.id, cmd.cmd_source, cmd.cmd_content "
                    " FROM icommands cmd "
                    " WHERE cmd.cmd_target = " + str::quoted(target) +
                    " AND cmd.cmd_executed = false "
                    " AND cmd_date + '15 sec'::interval > current_timestamp "
                    " ORDER BY cmd.id LIMIT 1;");

    try {
        result = runCmd(cmd);
        result &= (PQntuples(res) > 0);
        if (result) {
            id      = atoi(PQgetvalue(res, 0, 0));
            source  = std::string(PQgetvalue(res, 0, 1));
            content = std::string(PQgetvalue(res, 0, 2));
        }
    } catch(...) {
        result = false;
    }

    PQclear(res);
    return result;
}

//----------------------------------------------------------------------
// Method: markICommandAsDone
// Sets the executed flag to true
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::markICommandAsDone(int id)
{
    bool result = true;
    std::string cmd("UPDATE icommands SET cmd_executed = true "
                    " WHERE id = " + str::quoted(str::toStr<int>(id)) + ";");

    try { result = runCmd(cmd); } catch(...) { result = false; }

    PQclear(res);
    return result;
}

//----------------------------------------------------------------------
// Method: removeICommand
// Remove the command using its id
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::removeICommand(int id)
{
    bool result = true;
    std::string cmd("DELETE FROM icommands "
                    " WHERE id = " + str::quoted(str::toStr<int>(id)) + ";");

    try { result = runCmd(cmd); } catch(...) { result = false; }

    return result;
}

//----------------------------------------------------------------------
// Method: storeMsg
// Stores a message into the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::storeMsg(std::string from,
                               MessageString & msg,
                               bool isBroadcast)
{
    bool result = true;
    if (from.empty()) { from = "all"; }

    MessageBase m(msg);
    std::string cmd("INSERT INTO transmissions "
                    "(msg_date, msg_from, msg_to, msg_type, msg_bcast, msg_content) "
                    "VALUES (" +
                    str::quoted(str::tagToTimestamp(timeTag())) + ", " +
                    str::quoted(from) + ", " +
                    str::quoted(m.header.target()) + ", " +
                    str::quoted(m.header.type()) + ", " +
                    str::quoted(isBroadcast ? "Y" : "N") + ", " +
                    str::quoted(msg) + ")");

    try { result = runCmd(cmd); } catch(...) { throw; }

    return result;
}

//----------------------------------------------------------------------
// Method: retrieveMsgs
// Retrieves a set of messages from the database, according to
// pre-defined criteria
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::retrieveMsgs(std::vector<std::pair<std::string,
                                   MessageString> > & msgList,
                                   std::string criteria)
{
    bool result = true;

    std::string cmd("SELECT m.msg_date, m.msg_content "
                    "FROM transmissions as m "
                    "ORDER BY m.msg_date ");
    cmd += criteria + ";";

    try { result = runCmd(cmd); } catch(...) { throw; }

    // Transfer the data to the msgList argument
    msgList.clear();
    int nRows = PQntuples(res);
    for (int i = 0; i < nRows; ++i) {
        std::string msg_date(PQgetvalue(res, i, 1));
        MessageString m = std::string(PQgetvalue(res, i, 2));
        msgList.push_back(std::make_pair(msg_date, m));
    }
    PQclear(res);

    return result;
}

//----------------------------------------------------------------------
// Method: getTable
// Get the content (all the records) from a given table
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::getTable(std::string tName,
                               std::vector< std::vector<std::string> > & table)
{
    return  (runCmd(std::string("SELECT * FROM " + tName)) && fillWithResult(table));
}

//----------------------------------------------------------------------
// Method: getQuery
// Get the result (all the records) of a given query
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::getQuery(std::string qry,
                               std::vector< std::vector<std::string> > & table)
{
    return  (runCmd(qry) && fillWithResult(table));
}

//----------------------------------------------------------------------
// Method: getNumRowsInTable
// Get the content (all the records) from a given table
//----------------------------------------------------------------------
int DBHdlPostgreSQL::getNumRowsInTable(std::string tName)
{
    int nRows = -1;
    if (runCmd(std::string("SELECT COUNT(*) FROM " + tName))) {
        nRows = atoi(PQgetvalue(res, 0, 0));
    }
    PQclear(res);
    return nRows;
}

//----------------------------------------------------------------------
// Method: runCmd
// Runs a SQL command
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::runCmd(std::string cmd)
{
    // Run the command
    res = PQexec(conn, cmd.c_str());
    if ((PQresultStatus(res) != PGRES_COMMAND_OK) &&
            (PQresultStatus(res) != PGRES_TUPLES_OK)) {
        std::string msg = (std::string("Failed cmd '" + cmd + "': ") +
                           std::string(PQerrorMessage(conn)));
        PQclear(res);
        PQfinish(conn);
        throw RuntimeException(msg);
    }
    return true;
}

//----------------------------------------------------------------------
// Method: fillWithResult
// Retrieves the content of the returned n-tuples (after and SQL command)
// into the table parameter
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::fillWithResult(std::vector< std::vector<std::string> > & table)
{
    // Transfer the data to the table argument
    table.clear();
    int nFields = PQnfields(res);
    int nRows = PQntuples(res);
    for (int i = 0; i < nRows; ++i) {
        std::vector<std::string> row;
        for (int j = 0; j < nFields; ++j) {
            row.push_back(std::string(PQgetvalue(res, i, j)));
        }
        table.push_back(row);
    }
    PQclear(res);

    return true;
}

//----------------------------------------------------------------------
// Method: quotedValue
// Returns the value used in the update string
//----------------------------------------------------------------------
std::string DBHdlPostgreSQL::eqKeyValue(std::string k, int x) 
{ return k + " = " + std::to_string(x); }

std::string DBHdlPostgreSQL::eqKeyValue(std::string k, double x) 
{ return k + " = " + std::to_string(x); }

std::string DBHdlPostgreSQL::eqKeyValue(std::string k, std::string x) 
{ return k + " = " + str::quoted(x); }

std::string DBHdlPostgreSQL::eqKeyValue(std::string k, Json::Value x) 
{   Json::StyledWriter writer;
    return k + " = " + str::quoted(writer.write(x)); }

//----------------------------------------------------------------------
// Method: updateTable
// Method to update a series of fields of a table
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::updateTable(std::string table, std::string cond,
                                  std::vector<std::string> & newValues)
{
    std::string newValStr = newValues.at(0);
    for (int i = 1; i < newValues.size(); ++i) {
        newValStr += ", " + newValues.at(i);
    }
    std::string cmd("UPDATE " + table + " SET " + newValStr);
    if (!cond.empty()) { cmd += " WHERE " + cond; }
    TRC("PSQL> " << cmd);
    return runCmd(cmd);
}

//----------------------------------------------------------------------
// Method: getVersionCounter
// Returns the process version counter for a given processor
//----------------------------------------------------------------------
int DBHdlPostgreSQL::getVersionCounter(std::string & procName)
{
    bool result;
    std::string cmd("SELECT counter FROM pvc "
                    "WHERE name LIKE " +
                    str::quoted(procName + "%") + " ORDER BY id "
                    "DESC LIMIT 1;");

    try { result = runCmd(cmd); } catch(...) { throw; }

    int cnt = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    return cnt;
}

//----------------------------------------------------------------------
// Method: checkSignature
// Check if a product with the same signature exists in the archive
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::checkSignature(std::string & sgnt, std::string & ver)
{
    bool result = true;

    std::string cmd("SELECT product_version FROM products_info "
                    "WHERE signature LIKE " + str::quoted(sgnt + "%") +
                    " AND (NOW() - registration_time) > INTERVAL '10 sec')"
                    " ORDER BY id "
                    "DESC LIMIT 1;");

    // The 10 sec of margin are taken to avoid to count as existing, by
    // the TskOrc, products that where just inserted, by the DataMng,
    // into the local archive, since TskOrc and DataMng are receiving
    // almost at the same time the same INDATA message

    try {
        result = runCmd(cmd);
        result = PQntuples(res) > 0;
        if (result) {
            ver = std::string(PQgetvalue(res, 0, 0));
        }
    } catch(...) {
        throw;
    }

    PQclear(res);
    return result;
}


//}
