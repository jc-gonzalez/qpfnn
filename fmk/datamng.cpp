/******************************************************************************
 * File:    datamng.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.datamng
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
 *   Implement DataMng class
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

#include "datamng.h"

#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <utility>

#include "log.h"
#include "tools.h"

#include "filenamespec.h"
#include "dbhdlpostgre.h"
#include "except.h"
#include "config.h"
#include "dbg.h"
#include "str.h"
#include "urlhdl.h"
#include "channels.h"
#include "message.h"
#include "config.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
DataMng::DataMng(const char * name, const char * addr, Synchronizer * s)
    : Component(name, addr, s)
{
}

//----------------------------------------------------------------------
// Method: fromInitialisedToRunning
//----------------------------------------------------------------------
DataMng::DataMng(std::string name, std::string addr, Synchronizer * s)
    : Component(name, addr, s)
{
}

//----------------------------------------------------------------------
// Method: processCmdMsg
//----------------------------------------------------------------------
void DataMng::processCmdMsg(ScalabilityProtocolRole * conn, MessageString & m)
{
    JValue msg(m);
    std::string cmd = msg["cmd"].asString();

    if (cmd == "QUIT") {
        transitTo(RUNNING);
    }
}

//----------------------------------------------------------------------
// Method: processInDataMsg
//----------------------------------------------------------------------
void DataMng::processInDataMsg(ScalabilityProtocolRole * conn, MessageString & m)
{
    Message<MsgBodyINDATA> msg(m);
    MsgBodyINDATA & body = msg.body;
    ProductList prodList(body["products"]);

    // Synthetic INDATA messages, that means reading products from folder
    DBG("Transfer files to local archive");
    URLHandler urlh;
    for (auto & m : prodList.products) {
        urlh.setProduct(m);
        m = urlh.fromInbox2LocalArch();
    }

    // Save to DB
    DBG("Store products in DB");
    saveProductsToDB(prodList);
    DBG("Processing of DB done.");
}

//----------------------------------------------------------------------
// Method: processTskSchedMsg
//----------------------------------------------------------------------
void DataMng::processTskSchedMsg(ScalabilityProtocolRole * conn, MessageString & m)
{
    // Save task to DB
    saveTaskToDB(m, true);
}

//----------------------------------------------------------------------
// Method: processTskRepMsg
//----------------------------------------------------------------------
void DataMng::processTskRepMsg(ScalabilityProtocolRole * conn, MessageString & m)
{
    // Save task to DB
    saveTaskToDB(m);
}

//----------------------------------------------------------------------
// Method: initializeDB
// Initialize the DB
// (currently just an INI text file)
//----------------------------------------------------------------------
void DataMng::initializeDB()
{
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);

    // Check that connection with the DB is possible
    try {
        dbHdl->openConnection();
    } catch (RuntimeException & e) {
        ErrMsg(e.what());
        return;
    }

    // Close connection
    dbHdl->closeConnection();
}

//----------------------------------------------------------------------
// Method: saveTaskToDB
// Save the information on generated output products to the archive
//----------------------------------------------------------------------
void DataMng::saveTaskToDB(MessageString & m, bool initialStore)
{
    if (m.at(m.size() - 1) < 32) { m[m.size() - 1] = 0; }

    Message<MsgBodyTSK> msg(m);
    TaskInfo taskInfo(msg.body["info"]);

    // Save task information in task_info table
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);

    try {
        // Check that connection with the DB is possible
        dbHdl->openConnection();

        // Try to store the task data into the DB
        if (initialStore) { dbHdl->storeTask(taskInfo); }
        else              { dbHdl->updateTask(taskInfo); }

    } catch (RuntimeException & e) {
        ErrMsg(e.what());
        return;
    }

    // Close connection
    dbHdl->closeConnection();

    // In case the task has finished, save output products metadata
    if (taskInfo.taskStatus() == TASK_FINISHED) {

        URLHandler urlh;

        // Check version of products in gateway against DB
        sanitizeProductVersions(taskInfo.outputs);

        // Move products to local archive
        DBG("Preparing to send new INDATA with outputs to TskOrc...");
        for (auto & m : taskInfo.outputs.products) {
            urlh.setProduct(m);
            m = urlh.fromGateway2LocalArch();
        }

        InfoMsg("Saving outputs...");
        saveProductsToDB(taskInfo.outputs);

        InfoMsg("Sending message to register outputs at Orchestrator catalogue");

        Config & cfg = Config::_();
        if (cfg.flags.sendOutputsToMainArchive()) {
            InfoMsg("Archiving/Registering data at DSS/EAS");
            //std::thread(&DataMng::archiveDSSnEAS, this, std::ref(taskInfo..outputs)).detach();
            archiveDSSnEAS(taskInfo.outputs);
        }
    }
}

//----------------------------------------------------------------------
// Method: sanitizeProductVersions
// Make sure that there is no product with the same signature (and version)
// in local archive, changing the version if needed
//----------------------------------------------------------------------
void DataMng::sanitizeProductVersions(ProductList & prodList)
{
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);

    try {

        // Check that connection with the DB is possible
        dbHdl->openConnection();

        // Check signature for each product
        std::string ver;
        FileNameSpec fs;

        for (auto & m : prodList.products) {
            std::string sgnt = m.signature();
            std::cerr << "Checking signature " << sgnt << std::endl;
            if (dbHdl->checkSignature(sgnt, ver)) {
                // Version exists: change minor version number
                std::string origVer = m.productVersion();
                std::string newVer  = fs.incrMinorVersion(origVer);

                std::string url(m.url());
                std::string oldFile(str::mid(url,7,1000));

                WarnMsg("Found in database:" + sgnt + " [" + ver +
                        "], changing " + origVer + " with " + newVer);

                std::vector<std::string> svec {m.url(), m.baseName(), m.productId(), m.baseName(), sgnt };
                for (auto & s: svec) { str::replaceAll(s, origVer, newVer); }

                int i = 0;
                m["url"]            = svec.at(i++);
                m["baseName"]       = svec.at(i++);
                m["productId"]      = svec.at(i++);
                m["signature"]      = svec.at(i++);

                m["productVersion"] = newVer;

                url = m.url();
                std::string newFile(str::mid(url,7,1000));
                if (rename(oldFile.c_str(), newFile.c_str()) != 0) {
                    WarnMsg("ERROR: Cannot rename file " + oldFile + " to " + newFile);
                }
            }
        }

    } catch (RuntimeException & e) {
        ErrMsg(e.what());
        for (auto & v: prodList.products) { ErrMsg(v.str()); }
        return;
    }

    // Close connection
    dbHdl->closeConnection();
}

//----------------------------------------------------------------------
// Method: saveProductsToDB
// Save the information of a new (incoming) product to the DB
// (currently just an INI text file)
//----------------------------------------------------------------------
void DataMng::saveProductsToDB(ProductList & productList)
{
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);

    try {
        // Check that connection with the DB is possible
        dbHdl->openConnection();
        // Try to store the data into the DB
        dbHdl->storeProducts(productList);
    } catch (RuntimeException & e) {
        ErrMsg(e.what());
        for (auto & v: productList.products) { ErrMsg(v.str()); }
        return;
    }

    // Close connection
    dbHdl->closeConnection();
}

//----------------------------------------------------------------------
// Method: archiveDSSnEAS
// Sends the information to the area where the corresponding daemon is
// looking for data to be sent to DSS/EAS
//----------------------------------------------------------------------
void DataMng::archiveDSSnEAS(ProductList & productList)
{
/*
    static bool firstTime = true;
    static std::string proxyUser;
    static std::string proxyHost;
    static std::string proxyDropbox;
    static std::string proxyUrl;

    if (firstTime) {
        Config & cfg = Config::_();
        proxyUser    = cfg.storage.archive.user;
        proxyHost    = cfg.storage.archive.address;
        proxyDropbox = cfg.storage.archive.path;
        proxyUrl     = proxyUser + "@" + proxyHost + ":" + proxyDropbox + "/";

        firstTime = false;
    }

    return;

    for (auto & kv : productList.productList) {
        ProductType & prodType = const_cast<ProductType&>(kv.first);
        ProductMetadata & md = kv.second;

        // Execute task driver
        pid_t pid = fork();
        switch (pid) {
        case -1:
            // Error, failed to fork()
            perror("fork");
            exit(EXIT_FAILURE);
        case 0: {
            // We are the child
            std::cerr << md.url << std::endl;
            std::string fileName(md.url.substr(7, md.url.length()-7));
            std::string subBox;
            if      (isLE1Product(prodType)) { subBox = "data/"; }
            else if (isLE1Metadata(prodType)) { subBox = "meta/"; }
            proxyUrl += subBox;

            // Hack to send small files instead of actual products
            std::string source(fileName);
            std::string target(proxyUrl);
//            if ((md.productType == "VIS_RAW") || (md.productType == "VIS_LE1")) {
//                source = "/tmp/dummy_zeros";
//                target = proxyUrl + md.productId + ".fits";
//            }
            char *procTaskCmdLine[] = { (char*)("/usr/bin/scp"),
                                        (char*)(source.c_str()),
                                        (char*)(target.c_str()), NULL };
            std::string cmdLine = "scp " + source + " " + target;
            DBG("Trying to execute: '" << cmdLine << "'");
            char *env[] = { (char*)("LD_LIBRARY_PATH="),
                            (char*)("PATH=/usr/bin:/bin:/opt/bin"),
                            NULL };
            execve(procTaskCmdLine[0], procTaskCmdLine, env);
            perror("scp");
            exit(EXIT_FAILURE);
        }
        default: {
            // We are the parent process
            int status;
            if (waitpid(pid, &status, 0) < 0 ) {
                perror("wait");
                exit(254);
            }
            if (WIFEXITED(status)) {
                status = WEXITSTATUS(status);
                if (status == 0) {
                    InfoMsg("Successful arrival at DSS/EAS proxy host");
                } else {
                    InfoMsg("Problems with arrival of data to DSS/EAS proxy host");
                }
            }
            if (WIFSIGNALED(status)) {
                printf("Process %d killed: signal %d%s\n",
                       pid, WTERMSIG(status),
                       WCOREDUMP(status) ? " - core dumped" : "");
                exit(1);
            }
        }
        }
    }
*/
}

//----------------------------------------------------------------------
// Method: isProductAvailable
// Returns TRUE if there is a product type like the requested in the DB
//----------------------------------------------------------------------
bool DataMng::isProductAvailable(std::string prodType)
{
    //TODO  // return (ini.exists("ProductsIdx", prodType));
    UNUSED(prodType);

    return true;
}

//----------------------------------------------------------------------
// Method: getProductLatest
// Returns TRUE if there is a product type like the requested in the DB
//----------------------------------------------------------------------
bool DataMng::getProductLatest(std::string prodType,
                                   Json::Value & prodMetadata)
{
    bool retVal = true;
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);

    try {
        // Check that connection with the DB is possible
        dbHdl->openConnection();

        // Try to store the data into the DB
        ProductList prodList;
        std::string criteria = "WHERE p.product_type = '" + prodType +
                               "' LIMIT 1";
        dbHdl->retrieveProducts(prodList, criteria);
        if (prodList.products.size() > 0) {
            prodMetadata = prodList.products.at(0).val();
            retVal = false;
        }
    } catch (RuntimeException & e) {
        ErrMsg(e.what());
        retVal = false;
    }

    // Close connection
    dbHdl->closeConnection();

    return retVal;
}

//}
