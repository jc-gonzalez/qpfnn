/******************************************************************************
 * File:    cntrmng.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.cntrmng
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
 *   Implement ContainerMng class
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

#include "cntrmng.h"

#include "process.h"
#include "str.h"

#include <cassert>

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
ContainerMng::ContainerMng()
{
}

//----------------------------------------------------------------------
// Method: createContainer
// Creates a container that executes the requested application
//----------------------------------------------------------------------
bool ContainerMng::createContainer(std::string img, std::vector<std::string> opts,
                                   std::map<std::string, std::string> maps,
                                   std::string exe, std::vector<std::string> args,
                                   std::string & containerId)
{
    procxx::process cnt("docker", "run");
    for (auto & o : opts) { cnt.add_argument(o); }
    for (auto & kv : maps) {
        cnt.add_argument("-v");
        cnt.add_argument(kv.first + ":" + kv.second);
    }

    std::string tmpFileName(std::tmpnam(nullptr));
    cnt.add_argument("--cidfile");
    cnt.add_argument(tmpFileName);

    cnt.add_argument(img);

    cnt.add_argument(exe);
    for (auto & a : args) { cnt.add_argument(a); }

    cnt.exec();
    cnt.wait();

    std::ifstream dockerIdFile(tmpFileName);
    std::string id;
    std::getline(dockerIdFile, id))

    return (cnt.code() == 0);
}

//----------------------------------------------------------------------
// Method: getContainerInfo
// Retrieves information about running container
//----------------------------------------------------------------------
bool ContainerMng::getContainerInfo(std::string id, std::stringstream & info)
{
    procxx::process cntInspect("docker", "inspect");
    cntInspect.add_argument(id);
    cntInspect.exec();

    info.str("");
    std::string line;
    while (std::getline(cntInspect.output(), line)) {
        info << line << std::endl;
        if (!cntInspect.running() ||
            !procxx::running(cntInspect.id()) ||
            !running(cntInspect)) {
            break;
        }
    }

    cntInspect.wait();
    return (cntInspect.code() == 0);
}

//----------------------------------------------------------------------
// Method: killContainer
// Stop a given container
//----------------------------------------------------------------------
bool ContainerMng::killContainer(std::string id)
{
    procxx::process srvRm("docker", "rm");
    srvRm.add_argument(id);
    srvRm.exec();
    srvRm.wait();
    return (srvRm.code() == 0);
}

//}
