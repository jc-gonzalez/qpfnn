/******************************************************************************
 * File:    config.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.Config
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
 *   Declare Config class
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

#ifndef CONFIG_H
#define CONFIG_H

//------------------------------------------------------------
// Topic: System dependencies
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project dependencies
//------------------------------------------------------------
#include "datatypes.h"
#include "procinfo.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

#define DEFAULT_INITIAL_PORT   50000

//==========================================================================
// Class: CfgGrpGeneral
//==========================================================================
class CfgGrpGeneral : public JRecord {
public:
    CfgGrpGeneral() {}
    CfgGrpGeneral(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTR(appName);
        DUMPJSTR(appVersion);
        DUMPJSTR(lastAccess);
        DUMPJSTR(workArea);
    }
    JSTR(appName);
    JSTR(appVersion);
    JSTR(lastAccess);
    JSTR(workArea);
};

//==========================================================================
// Class: CfgGrpSwarm
//==========================================================================
class CfgGrpSwarm : public JRecord {
public:
    CfgGrpSwarm() {}
    CfgGrpSwarm(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTRVEC(serviceNodes);
        DUMPJINT(scale);
        DUMPJSTR(name);
        DUMPJSTR(image);
        DUMPJSTR(exec);
        DUMPJSTRVEC(args);
    }
    JSTRVEC(serviceNodes);
    JINT(scale);
    JSTR(name);
    JSTR(image);
    JSTR(exec);
    JSTRVEC(args);
};

//==========================================================================
// Class: CfgGrpNetwork
//==========================================================================
class CfgGrpNetwork : public JRecord {
public:
    CfgGrpNetwork() {}
    CfgGrpNetwork(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTR(masterNode);
        DUMPJINT(startingPort);
        DUMPJSTRINTMAP(processingNodes);
        DUMPJSTRGRPMAP(CfgGrpSwarm, swarms);
    }
    JSTR(masterNode);
    JINT(startingPort);
    JSTRINTMAP(processingNodes);
    JSTRGRPMAP(CfgGrpSwarm, swarms);
};

//==========================================================================
// Class: CfgGrpDB
//==========================================================================
class CfgGrpDB : public JRecord {
public:
    CfgGrpDB() {}
    CfgGrpDB(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTR(host);
        DUMPJSTR(port);
        DUMPJSTR(name);
        DUMPJSTR(user);
        DUMPJSTR(pwd);
    }
    JSTR(host);
    JSTR(port);
    JSTR(name);
    JSTR(user);
    JSTR(pwd);
};

//==========================================================================
// Class: CfgGrpProducts
//==========================================================================
class CfgGrpProducts : public JRecord {
public:
    CfgGrpProducts() {}
    CfgGrpProducts(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTRVEC(productTypes);
        DUMPJSTR(parsingRegEx);
        DUMPJSTR(parsingAssign);
        DUMPJSTR(productIdTpl);
        DUMPJSTRSTRMAP(extensions);
    }
    JSTRVEC(productTypes);
    JSTR(parsingRegEx);
    JSTR(parsingAssign);
    JSTR(productIdTpl);
    JSTRSTRMAP(extensions);
};

//==========================================================================
// Class: CfgGrpRulesList
//==========================================================================
class CfgGrpRulesList : public JRecord {
public:
    CfgGrpRulesList() {}
    CfgGrpRulesList(json v) : JRecord(v) {}
    JSTRIDX(inputs);
    JSTRIDX(outputs);
    JSTRIDX(processing);
    JSTRIDX(condition);
    virtual void dump() {
        FOREACH(i) {
            DUMPJSTRIDX(i,inputs);
            DUMPJSTRIDX(i,outputs);
            DUMPJSTRIDX(i,processing);
            DUMPJSTRIDX(i,condition);
        }
    }
};

//==========================================================================
// Class: CfgGrpOrchestration
//==========================================================================
class CfgGrpOrchestration : public JRecord {
public:
    CfgGrpOrchestration() {}
    CfgGrpOrchestration(json v) : JRecord(v) {
        SET_GRP(CfgGrpRulesList, rules);
    }
    virtual void dump() {
        rules.dump();
        DUMPJSTRSTRMAP(processors);
    }
    GRP(CfgGrpRulesList, rules);
    JSTRSTRMAP(processors);
};

//==========================================================================
// Class: CfgGrpUserDefToolsList
//==========================================================================
class CfgGrpUserDefToolsList : public JRecord {
public:
    CfgGrpUserDefToolsList() {}
    CfgGrpUserDefToolsList(json v) : JRecord(v) {}
    virtual void dump() {
        FOREACH(i) {
            DUMPJSTRIDX(i,name);
            DUMPJSTRIDX(i,description);
            DUMPJSTRIDX(i,arguments);
            DUMPJSTRIDX(i,executable);
            DUMPJSTRIDX(i,productTypes);
        }
    }
    JSTRIDX(name);
    JSTRIDX(description);
    JSTRIDX(arguments);
    JSTRIDX(executable);
    JSTRIDX(productTypes);
};

//==========================================================================
// Class: CfgGrpFlags
//==========================================================================
class CfgGrpFlags : public JRecord {
public:
    CfgGrpFlags() {}
    CfgGrpFlags(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTRVEC(msgsToDisk);
        DUMPJSTRVEC(msgsToDb);
        DUMPJBOOL(notifyMsgArrival);
        DUMPJBOOL(groupTaskAgentLogs);
        DUMPJBOOL(allowReprocessing);
        DUMPJBOOL(intermediateProducts);
        DUMPJBOOL(sendOutputsToMainArchive);
    }
    JSTRVEC(msgsToDisk);
    JSTRVEC(msgsToDb);
    JBOOL(notifyMsgArrival);
    JBOOL(groupTaskAgentLogs);
    JBOOL(allowReprocessing);
    JBOOL(intermediateProducts);
    JBOOL(sendOutputsToMainArchive);
};

//==========================================================================
// Class: Config
//==========================================================================
class Config : public JRecord {
public:
    static Config & _();
/*
    Config(std::string fName = std::string());
    Config(const char * fName = 0);
    Config(json v);
*/
    GRP(CfgGrpGeneral,          general);
    GRP(CfgGrpNetwork,          network);
    GRP(CfgGrpDB,               db);
    GRP(CfgGrpProducts,         products);
    GRP(CfgGrpOrchestration,    orchestration);
    GRP(CfgGrpUserDefToolsList, userDefTools);
    GRP(CfgGrpFlags,            flags);

    //----------------------------------------------------------------------
    // Method: init
    // Initializes the configuration data
    //----------------------------------------------------------------------
    void init(json v);

    //----------------------------------------------------------------------
    // Method: init
    // Initializes the configuration data
    //----------------------------------------------------------------------
    void init(std::string fName = std::string());

    //----------------------------------------------------------------------
    // Method: dump
    // Dumps config info to console
    //----------------------------------------------------------------------
    virtual void dump() {
        general.dump();
        network.dump();
        db.dump();
        products.dump();
        orchestration.dump();
        userDefTools.dump();
        flags.dump();
    }

    //----------------------------------------------------------------------
    // Method: setLastAccess
    // Updates the date of last access to the configuration
    //----------------------------------------------------------------------
    void setLastAccess(std::string last);

    //----------------------------------------------------------------------
    // Method: getEnvVar
    // Returns the content of an environment variable
    //----------------------------------------------------------------------
    std::string getEnvVar(std::string const & key) const;

    //----------------------------------------------------------------------
    // Method: setCurrentHostAddress
    //----------------------------------------------------------------------
    void setCurrentHostAddress(std::string & addr);

    //----------------------------------------------------------------------
    // Method: synchronizeSessionId
    //----------------------------------------------------------------------
    void synchronizeSessionId(std::string newId);

    //----------------------------------------------------------------------
    // Method: generateProcFmkInfoStructure
    //----------------------------------------------------------------------
    void generateProcFmkInfoStructure();

private:

    //----------------------------------------------------------------------
    // Method: setConfigFile
    // Set configuration file name
    //----------------------------------------------------------------------
    void setConfigFile(std::string fName);

    //----------------------------------------------------------------------
    // Method: readConfig
    // Loads the configuration file content into memory
    //----------------------------------------------------------------------
    void readConfigFromFile();

    //----------------------------------------------------------------------
    // Method: readConfigFromDB
    // Loads the configuration from the DB into memory
    //----------------------------------------------------------------------
    void readConfigFromDB();

    //----------------------------------------------------------------------
    // Method: saveConfigToDB
    // Store the configuration into the DB
    //----------------------------------------------------------------------
    void saveConfigToDB();

    //----------------------------------------------------------------------
    // Method: processConfig
    // Convert data in cfg (Json) to cfgInfo structure
    //----------------------------------------------------------------------
    void processConfig();

    //----------------------------------------------------------------------
    // Method: getRegExFromCfg
    // Retrieves filename regex from cfg or from designated file
    //----------------------------------------------------------------------
    std::string getRegExFromCfg(std::string & regexStr);

private:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    Config() {}

    //----------------------------------------------------------------------
    // Method: fillData
    //----------------------------------------------------------------------
    void fillData();

public:
    std::string           cfgFileName;
    std::string           cfgFilePath;
    std::string           sessionId;
    bool                  hmiPresent;
    bool                  isActualFile;
    std::string           currentHostAddr;
    bool                  weAreOnMaster;

    int                   startingPort;

    std::vector<std::string> agentNames;
    std::vector<std::string> agHost;
    std::vector<int>         agPortTsk;

    std::map<std::string, std::string> nodeStates;

    static std::map<std::string, AgentMode>   agentMode; 
    
    struct {
        std::string       inbox;
        std::string       archive;
        std::string       gateway;
        std::string       userArea;
    } storage;

    static std::string    DBHost;
    static std::string    DBPort;
    static std::string    DBName;
    static std::string    DBUser;
    static std::string    DBPwd;

    static std::string    PATHBase;
    static std::string    PATHData;
    static std::string    PATHRun;
    static std::string    PATHBin;
    static std::string    PATHSession;
    static std::string    PATHLog;
    static std::string    PATHRlog;
    static std::string    PATHTmp;
    static std::string    PATHTsk;
    static std::string    PATHMsg;

    static mode_t         PATHMode;

    static bool           isLive;

    static ProcessingFrameworkInfo * procFmkInfo;
};

namespace Configuration {
    static ::Config & cfg = ::Config::_();
}

//}

#endif  /* CONFIG_H */
