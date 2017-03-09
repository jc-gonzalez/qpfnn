/******************************************************************************
 * File:    urlhdl.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.urlhdl
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
 *   Implement URLHandler class
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

#include "urlhdl.h"

#include "str.h"

#include <unistd.h>
#include <ctime>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#include <libgen.h>

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//----------------------------------------------------------------------
// Method: Constructor
//----------------------------------------------------------------------
URLHandler::URLHandler(bool remote) : isRemote(remote)
{
}

//----------------------------------------------------------------------
// Method: fromExternal2Inbox
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromExternal2Inbox()
{
    ConfigurationInfo & cfgInfo = ConfigurationInfo::data();

    // NO DOWNLOAD IS STILL IMPLEMENTED
    // TODO: Implement download of external products into inbox

    // Get product basename
    std::vector<std::string> tokens = str::split(product.url, '/');
    std::string baseName = tokens.back();

    // Set new location and url
    std::string newFile(cfgInfo.storage.inbox.path + "/" + baseName);
    std::string newUrl ("file://" + newFile);

    // This method should only be called once the download has been done,
    // hence the only action left is setting the url
    DBG("Changing URL from " << product.url << " to " << newUrl);

    // Change url in processing task
    product.url = newUrl;
    product.urlSpace = InboxSpace;

    return product;
}

//----------------------------------------------------------------------
// Method: fromOutbox2External
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromOutbox2External()
{
    // NO UPLOAD IS STILL IMPLEMENTED
    // TODO: Implement upload of external products from outbox

    return product;
}

//----------------------------------------------------------------------
// Method: fromFolder2Inbox
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromFolder2Inbox()
{
    DBG(__FUNCTION__ << ':' << __LINE__);
    ConfigurationInfo & cfgInfo = ConfigurationInfo::data();

    assert(str::mid(product.url,0,8) == "file:///");
    assert((product.urlSpace == UserSpace) ||
           (product.urlSpace == ReprocessingSpace));

    // Get product basename
    std::vector<std::string> tokens = str::split(product.url, '/');
    std::string baseName = tokens.back();

    // Set new location and url
    std::string file(str::mid(product.url,7));
    std::string newFile(cfgInfo.storage.inbox.path + "/" + baseName);
    std::string newUrl ("file://" + newFile);

    // Set (hard) link (should it be move?)
    (void)relocate(file, newFile, LINK);

    // Change url in processing task
    product.url = newUrl;
    if (product.urlSpace != ReprocessingSpace) {
        product.urlSpace = InboxSpace;
    }

    return product;
}

//----------------------------------------------------------------------
// Method: fromInbox2Local
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromInbox2LocalArch()
{
    DBG(__FUNCTION__ << ':' << __LINE__);
    ConfigurationInfo & cfgInfo = ConfigurationInfo::data();

    assert(str::mid(product.url,0,8) == "file:///");
    assert((product.urlSpace == InboxSpace) ||
           (product.urlSpace == ReprocessingSpace));

    // Set new location and url
    std::string file(str::mid(product.url,7,1000));
    std::string newFile(file);
    std::string newUrl(product.url);

    std::string section("/in");

    str::replaceAll(newFile,
                    cfgInfo.storage.inbox.path,
                    cfgInfo.storage.local_archive.path + section);
    str::replaceAll(newUrl,
                    cfgInfo.storage.inbox.path,
                    cfgInfo.storage.local_archive.path + section);

    if (product.hadNoVersion) {
        std::string a("Z." + product.extension);
        std::string b("Z_" + product.productVersion + "." + product.extension);

        str::replaceAll(product.url, a, b);
        str::replaceAll(newFile,     a, b);
        str::replaceAll(newUrl,      a, b);

        product.hadNoVersion = false;
    }

    if (product.urlSpace != ReprocessingSpace) {
        // Set (hard) link (should it be move?)
        (void)relocate(file, newFile, MOVE);
    } else {
        // From now on the addressed file will be the existing one in the
        // local archive, so we remove the existing (hard) link in the inbox
        (void)unlink(file.c_str());
    }
    // Change url in processing task
    product.url = newUrl;
    product.urlSpace = LocalArchSpace;

    return product;
}

//----------------------------------------------------------------------
// Method: fromLocal2Gateway
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromLocalArch2Gateway()
{
    DBG(__FUNCTION__ << ':' << __LINE__);
    ConfigurationInfo & cfgInfo = ConfigurationInfo::data();

    assert(str::mid(product.url,0,8) == "file:///");
    assert(product.urlSpace == LocalArchSpace);

    // Set new location and url
    std::string file(str::mid(product.url,7,1000));
    std::string newFile(file);
    std::string newUrl(product.url);

    std::string section((newUrl.find("/out/") != std::string::npos) ? "/out" : "/in");

    str::replaceAll(newFile,
                    cfgInfo.storage.local_archive.path + section,
                    cfgInfo.storage.gateway.path + "/in");
    str::replaceAll(newUrl,
                    cfgInfo.storage.local_archive.path + section,
                    cfgInfo.storage.gateway.path + "/in");

    // Set (hard) link
    (void)relocate(file, newFile, LINK);

    // Change url in processing task
    product.url = newUrl;
    product.urlSpace = GatewaySpace;

    return product;
}

//----------------------------------------------------------------------
// Method: fromGateway2Processing
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromGateway2Processing()
{
    DBG(__FUNCTION__ << ':' << __LINE__);
    ConfigurationInfo & cfgInfo = ConfigurationInfo::data();

    assert(str::mid(product.url,0,8) == "file:///");
    assert(product.urlSpace == GatewaySpace);

    // Set new location and url
    std::string file(str::mid(product.url,7,1000));
    std::string newFile(file);
    std::string newUrl(product.url);

    std::string section("/in");

    str::replaceAll(newFile,
                    cfgInfo.storage.gateway.path + section,
                    taskExchgDir + section);
    str::replaceAll(newUrl,
                    cfgInfo.storage.gateway.path + section,
                    taskExchgDir + section);

    if (isRemote) {
        (void)relocate(file, newFile, COPY_TO_REMOTE);
        unlink(file.c_str());
    } else {
        (void)relocate(file, newFile, MOVE);
    }

    // Change url in processing task
    product.url = newUrl;
    product.urlSpace = ProcessingSpace;

    return product;
}

//----------------------------------------------------------------------
// Method: fromProcessing2Gateway
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromProcessing2Gateway()
{
    DBG(__FUNCTION__ << ':' << __LINE__);
    ConfigurationInfo & cfgInfo = ConfigurationInfo::data();

    assert(str::mid(product.url,0,8) == "file:///");
    assert(product.urlSpace == ProcessingSpace);

    // Set new location and url
    std::string file(str::mid(product.url,7,1000));
    std::string newFile(file);
    std::string newUrl(product.url);

    // Get extension
    std::string extension = str::mid(file,file.find_last_of('.') + 1);
    std::string subdir = "/out"; //(extension == "log") ? "/log" : "/out";

    std::string section("/out");

    str::replaceAll(newFile,
                    taskExchgDir + subdir,
                    cfgInfo.storage.gateway.path + section);
    str::replaceAll(newUrl,
                    taskExchgDir + subdir,
                    cfgInfo.storage.gateway.path + section);

    if (isRemote) {
        (void)relocate(file, newFile, COPY_TO_MASTER);
        runlink(file);
    } else {
        (void)relocate(file, newFile, LINK);
    }

    // Change url in processing task
    product.url = newUrl;
    product.urlSpace = GatewaySpace;

    return product;
}

//----------------------------------------------------------------------
// Method: fromGateway2LocalArch
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromGateway2LocalArch()
{
    DBG(__FUNCTION__ << ':' << __LINE__);
    ConfigurationInfo & cfgInfo = ConfigurationInfo::data();

    assert(str::mid(product.url,0,8) == "file:///");
    assert(product.urlSpace == GatewaySpace);

    // Set new location and url
    std::string file(str::mid(product.url,7,1000));
    std::string newFile(file);
    std::string newUrl(product.url);

    std::string section("/out");

    str::replaceAll(newFile,
                    cfgInfo.storage.gateway.path + section,
                    cfgInfo.storage.local_archive.path + section);
    str::replaceAll(newUrl,
                    cfgInfo.storage.gateway.path + section,
                    cfgInfo.storage.local_archive.path + section);

    // Set (hard) link
    (void)relocate(file, newFile, MOVE);

    // Change url in processing task
    product.url = newUrl;
    product.urlSpace = LocalArchSpace;

    return product;
}

//----------------------------------------------------------------------
// Method: relocate
//----------------------------------------------------------------------
int URLHandler::relocate(std::string & sFrom, std::string & sTo,
                         LocalArchiveMethod method, int msTimeOut)
{
    if (msTimeOut > 0) {
        struct stat buffer;
        struct timespec tsp1, tsp2;
        long elapsed = 0;
        (void)clock_gettime(CLOCK_REALTIME_COARSE, &tsp1);
        while ((stat(sFrom.c_str(), &buffer) != 0) || (elapsed > msTimeOut)) {
            (void)clock_gettime(CLOCK_REALTIME_COARSE, &tsp2);
            elapsed = ((tsp2.tv_sec - tsp1.tv_sec) * 1000 +
                       (tsp2.tv_nsec - tsp1.tv_nsec) / 1000000);
        }
    }

    int retVal = 0;
    switch(method) {
    case LINK:
        retVal = link(sFrom.c_str(), sTo.c_str());
        DBG("LINK: Hard link of " << sFrom << " to " << sTo);
        break;
    case SYMLINK:
        retVal = symlink(sFrom.c_str(), sTo.c_str());
        DBG("SYMLINK: Soft link of " << sFrom << " to " << sTo);
        break;
    case MOVE:
        retVal = rename(sFrom.c_str(), sTo.c_str());
        DBG("MOVE: Moving file from " << sFrom << " to " << sTo);
        break;
    case COPY:
        retVal = copyfile(sFrom, sTo);
        DBG("COPY: Copying file from " << sFrom << " to " << sTo);
        break;
    case COPY_TO_REMOTE:
    case COPY_TO_MASTER:
        retVal = rcopyfile(sFrom, sTo, method == COPY_TO_REMOTE);
        DBG(((method == COPY_TO_REMOTE) ? "COPY_TO_REMOTE: " : "COPY_TO_MASTER: ")
            << "Transferring file from " << sFrom << " to " << sTo);
        break;
    default:
        break;
    }

    if (retVal != 0) {
        perror((std::string("ERROR relocating product: ") +
                sFrom + std::string(" => ") + sTo).c_str());
        showBacktrace();
    }
    return retVal;
}

//----------------------------------------------------------------------
// Method: copyfile
//----------------------------------------------------------------------
int URLHandler::copyfile(std::string & sFrom, std::string & sTo)
{
    int source = open(sFrom.c_str(), O_RDONLY, 0);
    int dest = open(sTo.c_str(), O_WRONLY | O_CREAT, 0644);

    // struct required, rationale: function stat() exists also
    struct stat stat_source;
    fstat(source, &stat_source);

    sendfile(dest, source, 0, stat_source.st_size);

    close(source);
    close(dest);

    return 0;
}

//----------------------------------------------------------------------
// Method: rcopyfile
//----------------------------------------------------------------------
int URLHandler::rcopyfile(std::string & sFrom, std::string & sTo,
                          bool toRemote)
{
    static std::string scp("/usr/bin/scp");
    std::string cmd;
    if (toRemote) {
        cmd = scp + " " + master_address + ":" + sFrom + " " + sTo;
    } else {
        cmd = scp + " " + sFrom + " " + master_address + ":" + sTo;
    }
    DBG("CMD: " << cmd);
    int res = system(cmd.c_str());
    (void)(res);

    return 0;
}

//----------------------------------------------------------------------
// Method: runlink
//----------------------------------------------------------------------
int URLHandler::runlink(std::string & f)
{
    std::string cmd;
    cmd = "ssh " + master_address + " rm " + f;
    DBG("CMD: " << cmd);
    int res = system(cmd.c_str());
    (void)(res);

    return 0;
}

//----------------------------------------------------------------------
// Method: setRemoteCopyParams
//----------------------------------------------------------------------
void URLHandler::setRemoteCopyParams(std::string maddr, std::string raddr)
{
    master_address = maddr;
    remote_address = raddr;
    isRemote = true;
    DBG("Master addr: " << maddr << "  Remote addr: " << raddr);
}

//----------------------------------------------------------------------
// Method: setProcElemRunDir
//----------------------------------------------------------------------
void URLHandler::setProcElemRunDir(std::string wkDir, std::string tskDir)
{
    workDir = wkDir;
    intTaskDir = tskDir;

    taskExchgDir = workDir + "/" + intTaskDir;
    DBG("Workdir: " << workDir << "   IntTaskDir: " << intTaskDir << "  => TaskExchgDir: " << taskExchgDir);
}

//}
