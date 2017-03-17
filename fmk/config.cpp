/******************************************************************************
 * File:    config.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.config
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

#include <sys/time.h>
#include <fstream>
#include <iostream>
#include <libgen.h>

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
// Constructor
//----------------------------------------------------------------------
void Config::init(json v)
{
    value = v;
    {
        SET_GRP(CfgGrpGeneral,          general);
        SET_GRP(CfgGrpNetwork,          network);
        SET_GRP(CfgGrpDB,               db);
        SET_GRP(CfgGrpProducts,         products);
        SET_GRP(CfgGrpOrchestration,    orchestration);
        SET_GRP(CfgGrpUserDefToolsList, userDefTools);
        SET_GRP(CfgGrpFlags,            flags);
    }

    DBHost = db.host();
    DBPort = db.port();
    DBName = db.name();
    DBUser = db.user();
    DBPwd  = db.pwd();

    DBG(DBUser << ":" << DBPwd << "@" << DBHost << ":" << DBPort << "/" << DBName);

    isActualFile = false;

    processConfig();
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
void Config::init(std::string fName)
{
    isLive = false;
    DBG("Provided fName='" << fName << "'");
    if (fName.compare(0,5,"db://") == 0) {
        DBG("A database URL!");

        // fName in fact is a db url in the form:
        //   db://user:pwd@host:port/dbname
        // So, take the fields
        std::string url = fName.substr(5);
        DBUser = url.substr(0, url.find(":")); url.erase(0, url.find(":") + 1); // take username
        DBPwd  = url.substr(0, url.find("@")); url.erase(0, url.find("@") + 1); // take password
        DBHost = url.substr(0, url.find(":")); url.erase(0, url.find(":") + 1); // take host
        DBPort = url.substr(0, url.find("/")); url.erase(0, url.find("/") + 1); // take port
        DBName = url; // take database name

        DBG(DBUser << ":" << DBPwd << "@" << DBHost << ":" << DBPort << "/" << DBName);

        fName = ""; // clear filename, to read from DB
    }
    if (! fName.empty()) {
        setConfigFile(std::string(fName));
        readConfigFromFile();
        saveConfigToDB();
    } else {
        readConfigFromDB();
    }
    isLive = true;
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
/*
    char actualpath [PATH_MAX+1];
    char * ptr;
    ptr = realpath(fName.c_str(), actualpath);
    cfgFileName = std::string(ptr);
    cfgFilePath = std::string(dirname(ptr));
*/}

//----------------------------------------------------------------------
// Method: readConfigFromFile
// Loads the config file content into memory
//----------------------------------------------------------------------
void Config::readConfigFromFile()
{
/*    std::ifstream cfgJsonFile(cfgFileName);
    if (!cfgJsonFile.good()) {
        cfgJsonFile.close();
        return;
    }
    Json::Reader reader;
    reader.parse(cfgJsonFile, cfg);
    cfgJsonFile.close();

    isActualFile = true;
    processConfig();
*/}

//----------------------------------------------------------------------
// Method: readConfigFromDB
// Loads the config from the DB into memory
//----------------------------------------------------------------------
void Config::readConfigFromDB()
{
/*    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);
    dbHdl->setDbHost(Config::DBHost);
    dbHdl->setDbPort(Config::DBPort);
    dbHdl->setDbName(Config::DBName);
    dbHdl->setDbUser(Config::DBUser);
    dbHdl->setDbPasswd(Config::DBPwd);

    // Check that connection with the DB is possible
    try {
        dbHdl->openConnection();
    } catch (RuntimeException & e) {
        LibComm::Log::log("SYSTEM", Log::FATAL, e.what());
        return;
    }

    // Try to retrieve the config from the DB
    std::vector< std::vector<std::string> > config;
    std::string dateCreated;

    try {
        dbHdl->getTable("config", config);

        // Transfer DB config info to JSON value
        cfg.clear();
        unsigned int lastConfig = config.size() - 1;
        Json::Reader reader;
        reader.parse(config.at(lastConfig).at(1), cfg);
        dateCreated = config.at(lastConfig).at(0);
        cfgFileName = "<internalDB> " + Config::DBName + "::config";
    } catch (RuntimeException & e) {
        LibComm::Log::log("SYSTEM", Log::ERROR, e.what());
        return;
    } catch (...) {
        LibComm::Log::log("SYSTEM", Log::ERROR,
                          "Unexpected error accessing "
                          "database for retrieval of system config");
        return;
    }

    // Modificar fecha de último accesso
    std::string now = LibComm::timeTag();
    std::string cmd("UPDATE config SET last_accessed = '" + now + "' "
                    "WHERE created='" + dateCreated + "'");

    try {
        dbHdl->runCmd(cmd);
    } catch (RuntimeException & e) {
        LibComm::Log::log("SYSTEM", Log::ERROR, e.what());
        return;
    } catch (...) {
        LibComm::Log::log("SYSTEM", Log::ERROR,
                          "Unexpected error accessing "
                          "database for retrieval of system config");
        return;
    }

    // Get session id
    if (ConfigInfo::data().session.empty()) {
        try {
            std::pair<std::string, std::string> sessionAndState;
            sessionAndState = dbHdl->getLatestState();
            sessionId = sessionAndState.first;
        } catch (...) {
            LibComm::Log::log("SYSTEM", Log::ERROR,
                              "Unexpected error accessing "
                              "database for retrieval of session name");
            return;
        }
    } else {
        sessionId = ConfigInfo::data().session;
    }

    // Close connection
    dbHdl->closeConnection();

    isActualFile = false;
    processConfig();
*/}

//----------------------------------------------------------------------
// Method: saveConfigToDB
// Store the config into the DB
//----------------------------------------------------------------------
void Config::saveConfigToDB()
{
/*    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);
    dbHdl->setDbHost(Config::DBHost);
    dbHdl->setDbPort(Config::DBPort);
    dbHdl->setDbName(Config::DBName);
    dbHdl->setDbUser(Config::DBUser);
    dbHdl->setDbPasswd(Config::DBPwd);

    // Check that connection with the DB is possible
    try {
        dbHdl->openConnection();
    } catch (RuntimeException & e) {
        LibComm::Log::log("SYSTEM", Log::FATAL, e.what());
        return;
    }

    // Modify cfg to store
    ConfigInfo & cfgInfo = ConfigInfo::data();
    Json::Value dbcfg(cfg);
    dbcfg["products"]["parsing_regex"] = cfgInfo.parsing_regex;

    // Transfer config from JSON value to DB
    std::string cmd;
    std::string now = LibComm::timeTag();
    Json::FastWriter writer;
    std::string cfgString = writer.write(dbcfg);

    cmd = "INSERT INTO config (created, last_accessed, cfg) VALUES ";
    cmd += "('" + now + "', '" + now + "', '" + cfgString + "')";

    try {
         dbHdl->runCmd(cmd);
    } catch (RuntimeException & e) {
        LibComm::Log::log("SYSTEM", Log::ERROR, e.what());
        return;
    } catch (...) {
        LibComm::Log::log("SYSTEM", Log::ERROR,
                          "Unexpected error accessing "
                          "database for storage of system config");
        return;
    }

    // Close connection
    dbHdl->closeConnection();
*/}

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

    storage.inbox   = PATHData + "/inbox";
    storage.archive = PATHData + "/archive";
    storage.gateway = PATHData + "/gateway";

/*
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);
    dbHdl->setDbHost(Config::DBHost);
    dbHdl->setDbPort(Config::DBPort);
    dbHdl->setDbName(Config::DBName);
    dbHdl->setDbUser(Config::DBUser);
    dbHdl->setDbPasswd(Config::DBPwd);

    ConfigInfo & cfgInfo = ConfigInfo::data();

    // START OF: Config Reading

    // Now, fill in ConfigInfo structure
    reset();
    cfgInfo.clear();
    cfgInfo.hmiPresent   = false;
    cfgInfo.isActualFile = isActualFile;

    // Config file name
    cfgInfo.currentMachine = getEnvVar("HOSTNAME");
    cfgInfo.currentUser    = getEnvVar("USER");
    cfgInfo.cfgFileName    = cfgFileName;

    // General
    getGeneralInfo(cfgInfo.appName, cfgInfo.appVersion, cfgInfo.lastAccess);

    // Product datatypes
    getProductTypes(cfgInfo.orcParams.productTypes);

    // File name convention parameters
    const Json::Value & prds = cfg["products"];
    cfgInfo.parsing_assign   = prds["parsing_assign"].asString();
    cfgInfo.product_id_tpl   = prds["product_id_tpl"].asString();
    cfgInfo.data_ext         = prds["data_ext"].asString();
    cfgInfo.meta_ext         = prds["meta_ext"].asString();
    cfgInfo.log_ext          = prds["log_ext"].asString();

    std::string parsing_regex_str = prds["parsing_regex"].asString();
    cfgInfo.parsing_regex    = getRegExFromCfg(parsing_regex_str);

    FileNameSpec fs(cfgInfo.parsing_regex, cfgInfo.parsing_assign);
    fs.setProductIdTpl(cfgInfo.product_id_tpl);

    // Orchestration rules file
    for (int i = 0; i < getNumOrchRules(); ++i) {
        Rule * rule = new Rule;
        getOrchRule(rule->name, rule->inputs,
                    rule->outputs, rule->processingElement,
                    rule->condition);
        cfgInfo.orcParams.rules.push_back(rule);
    }

    // Processors
    // Check that connection with the DB is possible
    try {
        dbHdl->openConnection();
    } catch (RuntimeException & e) {
        LibComm::Log::log("SYSTEM", Log::FATAL, e.what());
        return;
    }
    for (int i = 0; i < getNumProcs(); ++i) {
        Processor * pe = new Processor;
        getProc(pe->name, pe->exePath, pe->inPath, pe->outPath, pe->version);
        pe->counter = dbHdl->getVersionCounter(pe->name);
        cfgInfo.orcParams.processors[pe->name] = pe;
    }
    dbHdl->closeConnection();

    // Storage areas information
    const Json::Value & stge             = cfg["storage"];
    const Json::Value & stgeBase         = stge["base"];
    const Json::Value & stgeRun          = stge["run"];
    const Json::Value & stgeIn           = stge["incoming"];
    const Json::Value & stgeLocal        = stge["local_archive"];
    const Json::Value & stgeGatew        = stge["gateway"];
    const Json::Value & stgeArch         = stge["archive"];
    const Json::Value & stgeOut          = stge["outgoing"];

    cfgInfo.storage.base                 = stgeBase["path"].asString();
    cfgInfo.storage.run                  = stgeRun["path"].asString();
    cfgInfo.storage.local_archive.path   = stgeLocal["path"].asString();
    cfgInfo.storage.gateway.path         = stgeGatew["path"].asString();

    cfgInfo.storage.tasks                = PATHTsk;

    getExternalStorage(stgeIn,   cfgInfo.storage.inbox);
    getExternalStorage(stgeOut,  cfgInfo.storage.outbox);
    getExternalStorage(stgeArch, cfgInfo.storage.archive);

    // User Defined Tools
    for (int i = 0; i < getNumUserDefTools(); ++i) {
        UserDefTool udt;
        getUserDefTool(udt);
        cfgInfo.userDefTools[udt.name] = udt;
    }

    // Flags
    const Json::Value & flags = cfg["flags"];
    const Json::Value & monitFlags = flags["monitoring"];
    const Json::Value & procFlags  = flags["processing"];
    const Json::Value & archFlags  = flags["archiving"];

    Json::Value::iterator it = monitFlags["msgs_to_disk"].begin();
    while (it != monitFlags["msgs_to_disk"].end()) {
        Json::Value const & v = (*it);
        std::string msgName = v.asString();
        cfgInfo.flags.monit.msgsToDisk[msgName] = true;
        ++it;
    }
    it = monitFlags["msgs_to_db"].begin();
    while (it != monitFlags["msgs_to_db"].end()) {
        Json::Value const & v = (*it);
        std::string msgName = v.asString();
        cfgInfo.flags.monit.msgsToDB[msgName] = true;
        ++it;
    }
    cfgInfo.flags.monit.notifyMsgArrival         = monitFlags["notify_msg_arrival"].asBool();
    cfgInfo.flags.monit.groupTaskAgentLogs       = monitFlags["group_task_agent_logs"].asBool();

    cfgInfo.flags.proc.allowReprocessing         = procFlags["allow_reprocessing"].asBool();
    cfgInfo.flags.proc.intermedProducts          = procFlags["intermediate_products"].asBool();

    cfgInfo.flags.arch.sendOutputsToMainArchive  = archFlags["send_outputs_to_main_archive"].asBool();

    if (isLive) { return; }

    // Nodes
    for (int i = 0; i < getNumNodes(); ++i) {
        Peer * peer = new Peer;
        getNode(peer->name, peer->type, peer->clientAddr, peer->serverAddr);
        cfgInfo.peersCfg.push_back(*peer);
        cfgInfo.peerNames.push_back(peer->name);
        cfgInfo.peersCfgByName[peer->name] = peer;
        if (peer->type == "evtmng") {
            cfgInfo.evtMngCfg.name = peer->name;
            cfgInfo.evtMngCfg.type = peer->type;
            cfgInfo.evtMngCfg.clientAddr = peer->clientAddr;
            cfgInfo.evtMngCfg.serverAddr = peer->serverAddr;
        }
    }

    // HMI node
    cfgInfo.qpfhmiCfg.name = getHMINodeName();
    getNodeByName(cfgInfo.qpfhmiCfg.name,
                  cfgInfo.qpfhmiCfg.type,
                  cfgInfo.qpfhmiCfg.clientAddr,
                  cfgInfo.qpfhmiCfg.serverAddr);

    // Master node
    cfgInfo.masterMachine = cfg["nodes"]["master_machine"].asString();
    cfgInfo.isMaster = (cfgInfo.masterMachine == cfgInfo.currentMachine);

    // Machines and connections
    reset();
    std::string mname;
    std::vector<std::string> mnodes;
    for (int i = 0; i < getNumMachines(); ++i) {
        mnodes.clear();
        getMachine(mname, mnodes);
        cfgInfo.machines.push_back(mname);
        cfgInfo.machineNodes[mname] = mnodes;
    }

    for (unsigned int i = 0; i < cfgInfo.peerNames.size(); ++i) {
        std::vector<std::string> nconn;
        getConnectionsForNode(cfgInfo.peerNames.at(i), nconn);
        cfgInfo.connections[cfgInfo.peerNames.at(i)] = nconn;
    }

    // END OF: Config Reading

    // Create peer commnodes for nodes in current machine
    std::vector<std::string> & machineNodes =
            cfgInfo.machineNodes[cfgInfo.currentMachine];

    SHW("Creating connections:\n");

    for (unsigned int i = 0; i < machineNodes.size(); ++i) {

        Peer * peer = cfgInfo.peersCfgByName[machineNodes.at(i)];
        std::string & peerName = peer->name;
        std::string & peerType = peer->type;

        Component * component = createNewComponent(cfgInfo, peerType, peerName.c_str());
        if (component == 0) { continue; }

        component->addPeer(cfgInfo.peersCfgByName[peerName], true);
        //DBG("Creating connections for " << peerName
        //    << "  [" << peer->clientAddr
        //    << " ; " << peer->serverAddr << "]");
        SHW("* " << peerName << " [" << peer->clientAddr << "] <==>\n");

        std::vector<std::string> & connectNodes = cfgInfo.connections[peerName];

        for (unsigned int j = 0; j < connectNodes.size(); ++j) {
            Peer * otherPeer = cfgInfo.peersCfgByName[connectNodes.at(j)];
            component->addPeer(otherPeer);
            //DBG("  Connecting to " << otherPeer->name
            //    << "  [" << otherPeer->clientAddr
            //    << " ; " << otherPeer->serverAddr << "]");
            SHW("\t\t* " << otherPeer->name << " [" << otherPeer->clientAddr << "]\n");
        }

        cfgInfo.peerNodes.push_back(component);

    }
*/
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

//}
