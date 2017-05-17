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

#include <unistd.h>
#include <cassert>
#include <ctime>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#include <libgen.h>

#include "str.h"
#include "config.h"

#include "dbg.h"

#define showBacktrace()

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

using Configuration::cfg;

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
    // NO DOWNLOAD IS STILL IMPLEMENTED
    // TODO: Implement download of external products into inbox

    // Get product basename
    std::vector<std::string> tokens;
    productUrl = product.url();
    str::split(productUrl, '/', tokens);
    std::string baseName = tokens.back();

    // Set new location and url
    std::string newFile(cfg.storage.inbox + "/" + baseName);
    std::string newUrl ("file://" + newFile);

    // This method should only be called once the download has been done,
    // hence the only action left is setting the url
    TRC("Changing URL from " << productUrl << " to " << newUrl);

    // Change url in processing task
    product["url"]      = newUrl;
    product["urlSpace"] = InboxSpace;

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
    TRC(__FUNCTION__ << ':' << __LINE__);

    productUrl      = product.url();
    productUrlSpace = product.urlSpace();

    assert(str::mid(productUrl,0,8) == "file:///");
    assert((productUrlSpace == UserSpace) ||
           (productUrlSpace == ReprocessingSpace));

    // Get product basename
    std::vector<std::string> tokens;
    str::split(productUrl, '/', tokens);
    std::string baseName = tokens.back();

    // Set new location and url
    std::string file(str::mid(productUrl,7));
    std::string newFile(cfg.storage.inbox + "/" + baseName);
    std::string newUrl("file://" + newFile);

    // Set (hard) link (should it be move?)
    (void)relocate(file, newFile, LINK);

    // Change url in processing task
    product["url"] = newUrl;
    if (productUrlSpace != ReprocessingSpace) {
        product["urlSpace"] = InboxSpace;
    }

    return product;
}

//----------------------------------------------------------------------
// Method: fromInbox2Local
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromInbox2LocalArch(bool tx)
{
    TRC(__FUNCTION__ << ':' << __LINE__);

    productUrl      = product.url();
    productUrlSpace = product.urlSpace();

    assert(str::mid(productUrl,0,8) == "file:///");
    assert((productUrlSpace == InboxSpace) ||
           (productUrlSpace == ReprocessingSpace));

    // Set new location and url
    std::string file(str::mid(productUrl,7,1000));
    std::string newFile(file);
    std::string newUrl(productUrl);

    std::string section("/in");

    str::replaceAll(newFile,
                    cfg.storage.inbox,
                    cfg.storage.archive + section);
    str::replaceAll(newUrl,
                    cfg.storage.inbox,
                    cfg.storage.archive + section);

    if (product.hadNoVersion()) {
        std::string a("Z." + product.extension());
        std::string b("Z_" + product.productVersion() + "." + product.extension());

        str::replaceAll(productUrl,  a, b);
        str::replaceAll(newFile,     a, b);
        str::replaceAll(newUrl,      a, b);

        product["hadNoVersion"] = false;
    }

    if (tx) {
        if (productUrlSpace != ReprocessingSpace) {
            // Set (hard) link (should it be move?)
            (void)relocate(file, newFile, MOVE);
        } else {
            // From now on the addressed file will be the existing one in the
            // local archive, so we remove the existing (hard) link in the inbox
            (void)unlink(file.c_str());
        }
    }
    // Change url in processing task
    product["url"]      = newUrl;
    product["urlSpace"] = LocalArchSpace;

    return product;
}

//----------------------------------------------------------------------
// Method: fromLocal2Gateway
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromLocalArch2Gateway()
{
    TRC(__FUNCTION__ << ':' << __LINE__);

    productUrl      = product.url();
    productUrlSpace = product.urlSpace();

    assert(str::mid(productUrl,0,8) == "file:///");
    assert(productUrlSpace == LocalArchSpace);

    // Set new location and url
    std::string file(str::mid(productUrl,7,1000));
    std::string newFile(file);
    std::string newUrl(productUrl);

    std::string section((newUrl.find("/out/") != std::string::npos) ? "/out" : "/in");

    str::replaceAll(newFile,
                    cfg.storage.archive + section,
                    cfg.storage.gateway + "/in");
    str::replaceAll(newUrl,
                    cfg.storage.archive + section,
                    cfg.storage.gateway + "/in");

    // Set (hard) link
    (void)relocate(file, newFile, LINK);

    // Change url in processing task
    product["url"]      = newUrl;
    product["urlSpace"] = GatewaySpace;

    return product;
}

//----------------------------------------------------------------------
// Method: fromGateway2Processing
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromGateway2Processing()
{
    TRC(__FUNCTION__ << ':' << __LINE__);

    productUrl      = product.url();
    productUrlSpace = product.urlSpace();

    assert(str::mid(productUrl,0,8) == "file:///");
    assert(productUrlSpace == GatewaySpace);

    // Set new location and url
    std::string file(str::mid(productUrl,7,1000));
    std::string newFile(file);
    std::string newUrl(productUrl);

    std::string section("/in");

    str::replaceAll(newFile,
                    cfg.storage.gateway + section,
                    taskExchgDir + section);
    str::replaceAll(newUrl,
                    cfg.storage.gateway + section,
                    taskExchgDir + section);

    if (isRemote) {
        (void)relocate(file, newFile, COPY_TO_REMOTE);
        unlink(file.c_str());
    } else {
        (void)relocate(file, newFile, MOVE);
    }

    // Change url in processing task
    product["url"]      = newUrl;
    product["urlSpace"] = ProcessingSpace;

    return product;
}

//----------------------------------------------------------------------
// Method: fromProcessing2Gateway
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromProcessing2Gateway()
{
    TRC(__FUNCTION__ << ':' << __LINE__);

    productUrl      = product.url();
    productUrlSpace = product.urlSpace();

    assert(str::mid(productUrl,0,8) == "file:///");
    assert(productUrlSpace == ProcessingSpace);

    // Set new location and url
    std::string file(str::mid(productUrl,7,1000));
    std::string newFile(file);
    std::string newUrl(productUrl);

    // Get extension
    std::string extension = str::mid(file,file.find_last_of('.') + 1);
    std::string subdir = "/out"; //(extension == "log") ? "/log" : "/out";

    std::string section("/out");

    str::replaceAll(newFile,
                    taskExchgDir + subdir,
                    cfg.storage.gateway + section);
    str::replaceAll(newUrl,
                    taskExchgDir + subdir,
                    cfg.storage.gateway + section);

    if (isRemote) {
        (void)relocate(file, newFile, COPY_TO_MASTER);
        runlink(file);
    } else {
        (void)relocate(file, newFile, LINK);
    }

    // Change url in processing task
    product["url"]      = newUrl;
    product["urlSpace"] = GatewaySpace;

    return product;
}

//----------------------------------------------------------------------
// Method: fromGateway2LocalArch
//----------------------------------------------------------------------
ProductMetadata & URLHandler::fromGateway2LocalArch()
{
    TRC(__FUNCTION__ << ':' << __LINE__);

    productUrl      = product.url();
    productUrlSpace = product.urlSpace();

    assert(str::mid(productUrl,0,8) == "file:///");
    assert(productUrlSpace == GatewaySpace);

    // Set new location and url
    std::string file(str::mid(productUrl,7,1000));
    std::string newFile(file);
    std::string newUrl(productUrl);

    std::string section("/out");

    str::replaceAll(newFile,
                    cfg.storage.gateway + section,
                    cfg.storage.archive + section);
    str::replaceAll(newUrl,
                    cfg.storage.gateway + section,
                    cfg.storage.archive + section);

    // Set (hard) link
    (void)relocate(file, newFile, MOVE);

    // Change url in processing task
    product["url"]      = newUrl;
    product["urlSpace"] = LocalArchSpace;

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
        TRC("LINK: Hard link of " << sFrom << " to " << sTo);
        break;
    case SYMLINK:
        retVal = symlink(sFrom.c_str(), sTo.c_str());
        TRC("SYMLINK: Soft link of " << sFrom << " to " << sTo);
        break;
    case MOVE:
        retVal = rename(sFrom.c_str(), sTo.c_str());
        TRC("MOVE: Moving file from " << sFrom << " to " << sTo);
        break;
    case COPY:
        retVal = copyfile(sFrom, sTo);
        TRC("COPY: Copying file from " << sFrom << " to " << sTo);
        break;
    case COPY_TO_REMOTE:
    case COPY_TO_MASTER:
        retVal = rcopyfile(sFrom, sTo, method == COPY_TO_REMOTE);
        TRC(((method == COPY_TO_REMOTE) ? "COPY_TO_REMOTE: " : "COPY_TO_MASTER: ")
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
    TRC("CMD: " << cmd);
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
    TRC("CMD: " << cmd);
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
    TRC("Master addr: " << maddr << "  Remote addr: " << raddr);
}

//----------------------------------------------------------------------
// Method: setProcElemRunDir
//----------------------------------------------------------------------
void URLHandler::setProcElemRunDir(std::string wkDir, std::string tskDir)
{
    workDir = wkDir;
    intTaskDir = tskDir;

    taskExchgDir = workDir + "/" + intTaskDir;
    TRC("Workdir: " << workDir << "   IntTaskDir: " << intTaskDir << "  => TaskExchgDir: " << taskExchgDir);
}

//}
