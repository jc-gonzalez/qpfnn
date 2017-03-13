/******************************************************************************
 * File:    datamng.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.datamng
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
 *   Implement DataManager class
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

#include "message.h"

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
// Constructor
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
}

//----------------------------------------------------------------------
// Method: processInDataMsg
//----------------------------------------------------------------------
void DataMng::processInDataMsg(ScalabilityProtocolRole * conn, MessageString & m)
{
}

//----------------------------------------------------------------------
// Method: processTskSchedMsg
//----------------------------------------------------------------------
void DataMng::processTskSchedMsg(ScalabilityProtocolRole * conn, MessageString & m)
{
}

//----------------------------------------------------------------------
// Method: processCmdMsg
//----------------------------------------------------------------------
void DataMng::processCmdMsg(ScalabilityProtocolRole * conn, MessageString & m)
{
}

//----------------------------------------------------------------------
// Method: processInDataMsg
//----------------------------------------------------------------------
void DataMng::processInDataMsg(ScalabilityProtocolRole * conn, MessageString & m)
{
}

//----------------------------------------------------------------------
// Method: processTskSchedMsg
//----------------------------------------------------------------------
void DataMng::processTskSchedMsg(ScalabilityProtocolRole * conn, MessageString & m)
{
}

//----------------------------------------------------------------------
// Method: initializeDB
// Initialize the DB
// (currently just an INI text file)
//----------------------------------------------------------------------
void DataManager::initializeDB()
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
// Method: saveToDB
// Save the information of a new (incoming) product to the DB
// (currently just an INI text file)
//----------------------------------------------------------------------
void DataManager::saveToDB(Message_INDATA * msg)
{
    saveProductsToDB(msg->productsMetadata);
}

//----------------------------------------------------------------------
// Method: saveTaskToDB
// Save the information on generated output products to the archive
//----------------------------------------------------------------------
void DataManager::saveTaskToDB(Message_TASK_Processing * msg, bool initialStore)
{
    // Save task information in task_info table
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);

    try {
        // Check that connection with the DB is possible
        dbHdl->openConnection();

        // Try to store the task data into the DB
        if (initialStore) {
            dbHdl->storeTask(msg->task);
        } else {
            dbHdl->updateTask(msg->task);
        }
    } catch (RuntimeException & e) {
        ErrMsg(e.what());
        return;
    }

    // Close connection
    dbHdl->closeConnection();

    // In case the task has finished, save output products metadata
    if (msg->task.taskStatus == TASK_FINISHED) {

        URLHandler urlh;

        // Check version of products in gateway against DB
        sanitizeProductVersions(msg->task.outputs);

        // Move products to local archive
        DBG("Preparing to send new INDATA with outputs to TskOrc...");
        for (auto & md : msg->task.outputs.productList) {
            urlh.setProduct(md.second);
            md.second = urlh.fromGateway2LocalArch();
        }

        InfoMsg("Saving outputs...");
        saveProductsToDB(msg->task.outputs);

        InfoMsg("Sending message to register outputs at Orchestrator catalogue");

        MessageHeader hdr;
        buildMsgHeader(MSG_INDATA_IDX, "DataMng", "TskOrc", hdr);
        Message_INDATA msgIn;
        buildMsgINDATA(hdr, msg->task.outputs, msgIn);

        // Send message
        PeerMessage * pmsgIn = buildPeerMsg(hdr.destination, msgIn.getDataString(), MSG_INDATA);
        registerMsg(selfPeer()->name, *pmsgIn);
        setTransmissionToPeer(hdr.destination, pmsgIn);

        ConfigurationInfo & cfgInfo = ConfigurationInfo::data();
        if (cfgInfo.flags.arch.sendOutputsToMainArchive) {
            InfoMsg("Archiving/Registering data at DSS/EAS");
            //std::thread(&DataManager::archiveDSSnEAS, this, std::ref(msg->task.outputs)).detach();
            archiveDSSnEAS(msg->task.outputs);
        }
    }
}

//----------------------------------------------------------------------
// Method: sanitizeProductVersions
// Make sure that there is no product with the same signature (and version)
// in local archive, changing the version if needed
//----------------------------------------------------------------------
void DataManager::sanitizeProductVersions(ProductCollection & prodList)
{
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);

    try {

        // Check that connection with the DB is possible
        dbHdl->openConnection();

        // Check signature for each product
        std::string ver;
        FileNameSpec fs;

        for (auto & kv : prodList.productList) {
            ProductMetadata & m = kv.second;
            std::cerr << "Checking signature " << m.signature << std::endl;
            if (dbHdl->checkSignature(m.signature, ver)) {
                // Version exists: change minor version number
                std::string origVer = m.productVersion;
                std::string newVer  = fs.incrMinorVersion(origVer);
                str::replaceAll(m.url, origVer, newVer);
                str::replaceAll(m.baseName, origVer, newVer);
                str::replaceAll(m.productId, origVer, newVer);
                str::replaceAll(m.signature, origVer, newVer);
                m.productVersion = newVer;
            }
        }

    } catch (RuntimeException & e) {
        ErrMsg(e.what());
        ErrMsg(prodList.getDataString());
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
void DataManager::saveProductsToDB(ProductCollection & productList)
{
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);

    try {

        // Check that connection with the DB is possible
        dbHdl->openConnection();

        // Try to store the data into the DB
        dbHdl->storeProducts(productList);

    } catch (RuntimeException & e) {
        ErrMsg(e.what());
        ErrMsg(productList.getDataString());
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
void DataManager::archiveDSSnEAS(ProductCollection & productList)
{
    static bool firstTime = true;
    static std::string proxyUser;
    static std::string proxyHost;
    static std::string proxyDropbox;
    static std::string proxyUrl;

    if (firstTime) {
        ConfigurationInfo & cfgInfo = ConfigurationInfo::data();
        proxyUser    = cfgInfo.storage.archive.user;
        proxyHost    = cfgInfo.storage.archive.address;
        proxyDropbox = cfgInfo.storage.archive.path;
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
}

//----------------------------------------------------------------------
// Method: isProductAvailable
// Returns TRUE if there is a product type like the requested in the DB
//----------------------------------------------------------------------
bool DataManager::isProductAvailable(std::string prodType)
{
    //TODO  // return (ini.exists("ProductsIdx", prodType));
    UNUSED(prodType);

    return true;
}

//----------------------------------------------------------------------
// Method: getProductLatest
// Returns TRUE if there is a product type like the requested in the DB
//----------------------------------------------------------------------
bool DataManager::getProductLatest(std::string prodType,
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
        if (prodList.productList.size() > 0) {
            prodMetadata = prodList.productList.at(0).getData();
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
