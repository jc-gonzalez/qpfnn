/******************************************************************************
 * File:    config.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.config
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
 *   Implement Config class
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

#include <climits>
#include <cstdlib>

#include "config.h"

#include "dbg.h"
#include "str.h"
#include "tools.h"
#include "dbhdlpostgre.h"
#include "except.h"
#include "log.h"

#include <sys/time.h>
#include <fstream>
#include <iostream>
#include <libgen.h>
#include <memory>

using Configuration::cfg;

#define WRITE_MESSAGE_FILES

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

Config & Config::_()
{
    static Config configInstance {};

    return configInstance;
}

//----------------------------------------------------------------------
// Method: init
//----------------------------------------------------------------------
void Config::init(json v)
{
    value = v;

    fillData();
    isActualFile = false;
    processConfig();
}

//----------------------------------------------------------------------
// Method: setCurrentHostAddress
//----------------------------------------------------------------------
void Config::setCurrentHostAddress(std::string & addr)
{
    currentHostAddr = addr;
}

//----------------------------------------------------------------------
// Method: init
//----------------------------------------------------------------------
void Config::init(std::string fName)
{
    isLive = false;
    sessionId = timeTag();
    TRC("Provided fName='" << fName << "'");
    if (fName.compare(0,5,"db://") == 0) {
        TRC("A database URL!");

        // fName in fact is a db url in the form:
        //   db://user:pwd@host:port/dbname
        // So, take the fields
        std::string url = fName.substr(5);
        DBUser = url.substr(0, url.find(":")); url.erase(0, url.find(":") + 1); // take username
        DBPwd  = url.substr(0, url.find("@")); url.erase(0, url.find("@") + 1); // take password
        DBHost = url.substr(0, url.find(":")); url.erase(0, url.find(":") + 1); // take host
        DBPort = url.substr(0, url.find("/")); url.erase(0, url.find("/") + 1); // take port
        DBName = url; // take database name

        TRC(DBUser << ":" << DBPwd << "@" << DBHost << ":" << DBPort << "/" << DBName);

        fName = ""; // clear filename, to read from DB
    }

    if (! fName.empty()) {
        TRC("Configuration is retrieved from file: " << fName);
        setConfigFile(fName);
        readConfigFromFile();
        if (weAreOnMaster) { saveConfigToDB(); }
        isActualFile = true;
    } else {
        TRC("Configuration is retrieved from db: " << fName);
        readConfigFromDB();
        isActualFile = false;
    }

    isLive = true;

    processConfig();
}

//----------------------------------------------------------------------
// Method: synchronizeSessionId
//----------------------------------------------------------------------
void Config::synchronizeSessionId(std::string newId)
{
    std::string newPATHSession(PATHRun + "/" + newId);

    if (rename(PATHSession.c_str(), newPATHSession.c_str()) != 0) {
        perror("Change sessionId");
        TRC("Cannot rename PATHSession from " + PATHSession +
            " to " + newPATHSession);
        WarnMsg("Cannot rename PATHSession from " + PATHSession +
                " to " + newPATHSession);
        return;
    }

    PATHSession = newPATHSession;

    PATHLog     = PATHSession + "/log";
    PATHRlog    = PATHSession + "/rlog";
    PATHTmp     = PATHSession + "/tmp";
    PATHTsk     = PATHSession + "/tsk";
    PATHMsg     = PATHSession + "/msg";
}

//----------------------------------------------------------------------
// Method: fillData
//----------------------------------------------------------------------
void Config::fillData()
{
    json & v = value;
    SET_GRP(CfgGrpGeneral,          general);
    SET_GRP(CfgGrpNetwork,          network);
    SET_GRP(CfgGrpDB,               db);
    SET_GRP(CfgGrpProducts,         products);
    SET_GRP(CfgGrpOrchestration,    orchestration);
    SET_GRP(CfgGrpUserDefToolsList, userDefTools);
    SET_GRP(CfgGrpFlags,            flags);

    DBHost = db.host();
    DBPort = db.port();
    DBName = db.name();
    DBUser = db.user();
    DBPwd  = db.pwd();

    TRC(DBUser << ":" << DBPwd << "@" << DBHost << ":" << DBPort << "/" << DBName);

    weAreOnMaster = (network.masterNode() == currentHostAddr);
}

//----------------------------------------------------------------------
// Method: setLastAccess
// Updates the date of last access to the config
//----------------------------------------------------------------------
void Config::setLastAccess(std::string last)
{
//    cfg["general"]["last_access"] = last;
}

//----------------------------------------------------------------------
// Method: setConfigFile
// Set config file name
//----------------------------------------------------------------------
void Config::setConfigFile(std::string fName)
{

    char actualpath [PATH_MAX+1];
    char * ptr;
    ptr = realpath(fName.c_str(), actualpath);
    cfgFileName = std::string(ptr);
    cfgFilePath = std::string(dirname(ptr));
    TRC("cfgFileName set to: " << cfgFileName);
    TRC("cfgFilePath set to: " << cfgFilePath);
}

//----------------------------------------------------------------------
// Method: readConfigFromFile
// Loads the config file content into memory
//----------------------------------------------------------------------
void Config::readConfigFromFile()
{
    std::ifstream cfgFile(cfgFileName);
    std::stringstream buffer;
    buffer << cfgFile.rdbuf();
    TRC("CONFIG FROM FILE:\n" + buffer.str());
    fromStr(buffer.str());
    fillData();
}

//----------------------------------------------------------------------
// Method: readConfigFromDB
// Loads the config from the DB into memory
//----------------------------------------------------------------------
void Config::readConfigFromDB()
{
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);
    dbHdl->setDbHost(Config::DBHost);
    dbHdl->setDbPort(Config::DBPort);
    dbHdl->setDbName(Config::DBName);
    dbHdl->setDbUser(Config::DBUser);
    dbHdl->setDbPasswd(Config::DBPwd);

    // Check that connection with the DB is possible
    try {
        dbHdl->openConnection();
    } catch (RuntimeException & e) {
        TRC("ERROR Trying to open connection to DB");
        Log::log("SYSTEM", Log::FATAL, e.what());
        return;
    }

    // Try to retrieve the config from the DB
    std::vector< std::vector<std::string> > config;
    std::string dateCreated;

    try {
        // Get config table and Transfer DB config info to JSON value
        dbHdl->getTable("configuration", config);
        unsigned int lastConfig = config.size() - 1;
        //Json::Reader reader;
            //reader.parse(config.at(lastConfig).at(1), cfg);
        dateCreated = config.at(lastConfig).at(0);
            std::string configData(config.at(lastConfig).at(1));
            TRC("Retrieved config. data: " << configData);
            cfg.fromStr(configData);
        cfgFileName = "<internalDB> " + Config::DBName + "::configuration";
    } catch (RuntimeException & e) {
            TRC("ERROR Trying to retrieve configuration table");
        Log::log("SYSTEM", Log::ERROR, e.what());
        return;
    } catch (...) {
            TRC("ERROR Trying to retrieve configuration table");
        Log::log("SYSTEM", Log::ERROR,
                          "Unexpected error accessing "
                          "database for retrieval of system configuration");
        return;
    }

    // Modificar fecha de último accesso
    std::string now = timeTag();
    std::string cmd("UPDATE configuration SET last_accessed = '" + now + "' "
                    "WHERE created='" + dateCreated + "'");

    try {
        dbHdl->runCmd(cmd);
    } catch (RuntimeException & e) {
        Log::log("SYSTEM", Log::ERROR, e.what());
        return;
    } catch (...) {
        Log::log("SYSTEM", Log::ERROR,
                          "Unexpected error accessing "
                          "database for retrieval of system configuration");
        return;
    }

    // Get session id
    if (sessionId.empty()) {
        try {
            std::pair<std::string, std::string> sessionAndState;
            sessionAndState = dbHdl->getLatestState();
            sessionId = sessionAndState.first;
            if (sessionId.empty()) {
                sessionId = timeTag();
            }
        } catch (...) {
            Log::log("SYSTEM", Log::ERROR,
                              "Unexpected error accessing "
                              "database for retrieval of session name");
            return;
        }
    }
    TRC("SessionId: " << sessionId);

    // Close connection
    dbHdl->closeConnection();
}

//----------------------------------------------------------------------
// Method: saveConfigToDB
// Store the config into the DB
//----------------------------------------------------------------------
void Config::saveConfigToDB()
{
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);
    dbHdl->setDbHost(Config::DBHost);
    dbHdl->setDbPort(Config::DBPort);
    dbHdl->setDbName(Config::DBName);
    dbHdl->setDbUser(Config::DBUser);
    dbHdl->setDbPasswd(Config::DBPwd);

    // Check that connection with the DB is possible
    try {
        dbHdl->openConnection();
    } catch (RuntimeException & e) {
        Log::log("SYSTEM", Log::FATAL, e.what());
        return;
    }

    // Modify cfg to store
    //ConfigInfo & cfgInfo = ConfigInfo::data();
    //Json::Value dbcfg(cfg);
    json dbcfg = cfg.val();
    dbcfg["products"]["parsing_regex"] = cfg.products.parsingRegEx();

    // Transfer configuration from JSON value to DB
    std::string cmd;
    std::string now = timeTag();
    Json::FastWriter writer;
    std::string cfgString = writer.write(dbcfg);

    cmd = "INSERT INTO configuration (created, last_accessed, cfg) VALUES ";
    cmd += "('" + now + "', '" + now + "', '" + cfgString + "')";

    try {
         dbHdl->runCmd(cmd);
    } catch (RuntimeException & e) {
        Log::log("SYSTEM", Log::ERROR, e.what());
        return;
    } catch (...) {
        Log::log("SYSTEM", Log::ERROR,
                          "Unexpected error accessing "
                          "database for storage of system config");
        return;
    }

    // Close connection
    dbHdl->closeConnection();
}

//----------------------------------------------------------------------
// Method: getRegExFromCfg
// Retrieves filename regex from cfg or from designated file
//----------------------------------------------------------------------
std::string Config::getRegExFromCfg(std::string & regexStr)
{
    /*
    // Regex for parsing file names might be in a separate file pointed by
    // parsing_regex parameter if the first character is '@'
    if (regexStr.at(0) == '@') {
        std::ifstream parsingReFile;
        parsingReFile.open(cfgFilePath + "/" + regexStr.substr(1),
                           std::ifstream::in);
        if (parsingReFile.good()) {
            std::string fileLine;
            do {
                std::getline(parsingReFile, fileLine);
            } while ((fileLine.length() < 1) ||
                    ((fileLine.at(0) == '#') ||
                    (fileLine.at(0) == '%') ||
                    (fileLine.at(0) == '\'') ||
                    (fileLine.at(0) == ':') ||
                    (fileLine.at(0) == '\t') ||
                    (fileLine.at(0) == ' ') ||
                    (fileLine.at(0) == '!')));
            parsingReFile.close();
            return fileLine;
        }
    } else {
        return regexStr;
    }
*/
}

//----------------------------------------------------------------------
// Method: processConfig
// Loads the config file content into memory
//----------------------------------------------------------------------
void Config::processConfig()
{
    PATHBase    = general.workArea();

    PATHData    = PATHBase + "/data";

    PATHRun     = PATHBase + "/run";

    PATHBin     = PATHRun + "/bin";
    PATHSession = PATHRun + "/" + sessionId;

    PATHLog     = PATHSession + "/log";
    PATHRlog    = PATHSession + "/rlog";
    PATHTmp     = PATHSession + "/tmp";
    PATHTsk     = PATHSession + "/tsk";
    PATHMsg     = PATHSession + "/msg";

    for (auto & p : std::vector<std::string>
        { PATHSession, PATHLog, PATHRlog,
                PATHTmp, PATHTsk, PATHMsg }) { TRC(p); }

    storage.inbox    = PATHData + "/inbox";
    storage.archive  = PATHData + "/archive";
    storage.gateway  = PATHData + "/gateway";
    storage.userArea = PATHData + "/user";
}

//----------------------------------------------------------------------
// Method: generateProcFmkInfoStructure
// Generates a new ProcFmkInfo structure
//----------------------------------------------------------------------
void Config::generateProcFmkInfoStructure()
{
    // Handy lambda to compute ports number, h=1:procHosts, i=0:agentsInHost
    // We will assume agentsInHost is < 10
    auto portnum = [](int start, int h, int i) -> int
        { return start + 10 * (h - 1) + i; };

    char sAgName[100];

    HostInfo hi;
    hi.update();
    int j = 0;

    std::vector<std::string> & agName    = cfg.agentNames;
    std::vector<std::string> & agHost    = cfg.agHost;
    std::vector<int> &         agPortTsk = cfg.agPortTsk;

    procFmkInfo->numContTasks = 0;
    procFmkInfo->numSrvTasks = 0;

    agName.clear();
    agPortTsk.clear();

    int h = 1;
    for (auto & ckv : cfg.network.processingNodes()) {
        int numOfTskAgents = ckv.second;
        hi.update();

        std::string ip = ckv.first;

        ProcessingHostInfo * ph = new ProcessingHostInfo;
        ph->name      = ip;
        ph->numAgents = numOfTskAgents;
        ph->hostInfo  = hi;
        ph->numTasks  = 0;

        for (int i = 0; i < ph->numAgents; ++i, ++j) {
            sprintf(sAgName, "TskAgent_%02d_%02d", h, i + 1);
            agName.push_back(std::string(sAgName));
            agPortTsk.push_back(portnum(startingPort + 10, h, i));

            AgentInfo agInfo;
            agInfo.name       = agName.at(j);
            agInfo.taskStatus = TaskStatusSpectra();
            agInfo.load       = (rand() % 1000) * 0.01;
            ph->agInfo.push_back(agInfo);
            ph->numTasks += agInfo.taskStatus.total;
        }

        procFmkInfo->hostsInfo[ph->name] = ph;
        procFmkInfo->numContTasks += ph->numTasks;
        agentMode[ip] = CONTAINER;
        ++h;
    }

    for (auto & skv : cfg.network.swarms()) {
        hi.update();
        CfgGrpSwarm & swrm = skv.second;
        std::string ip = swrm.serviceNodes().at(0);

        sprintf(sAgName, "Swarm_%s", ip.c_str());
        agName.push_back(std::string(sAgName));
        agPortTsk.push_back(portnum(startingPort + 10, h, 0));

        SwarmInfo * sw = new SwarmInfo;
        sw->name       = ip;
        sw->scale      = swrm.scale();
        sw->hostInfo   = hi;
        sw->taskStatus = TaskStatusSpectra();

        procFmkInfo->swarmInfo[ip] = sw;
        procFmkInfo->numSrvTasks += sw->scale;
        agentMode[ip] = SERVICE;
        ++h;
    }

    dump();
}

//----------------------------------------------------------------------
// Method: getEnvVar
// Returns the content of an environment variable
//----------------------------------------------------------------------
std::string Config::getEnvVar(std::string const & key) const
{
    char * val = std::getenv( key.c_str() );
    return val == NULL ? std::string("") : std::string(val);
}

std::string Config::DBHost;
std::string Config::DBPort;
std::string Config::DBName;
std::string Config::DBUser;
std::string Config::DBPwd;

std::string Config::PATHBase;
std::string Config::PATHData;
std::string Config::PATHRun;
std::string Config::PATHBin;
std::string Config::PATHSession;
std::string Config::PATHLog;
std::string Config::PATHRlog;
std::string Config::PATHTmp;
std::string Config::PATHTsk;
std::string Config::PATHMsg;

mode_t Config::PATHMode = 0755;

bool Config::isLive = false;

ProcessingFrameworkInfo * Config::procFmkInfo = new ProcessingFrameworkInfo;

std::map<std::string, AgentMode> Config::agentMode;

//}
