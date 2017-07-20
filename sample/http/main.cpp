/******************************************************************************
 * File:    main.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.main
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
 *   Implement TskMng class
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

#include <sys/time.h>
#include "httpserver.h"

using Configuration::cfg;

int main(int argc, char * argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <cfgFile>\n";
        exit(-1);
    }

    cfg.init(std::string(argv[1]));
    Log::setLogBaseDir(Config::PATHSession);

    // Launch the HTTP Server for Docker Swarm Services, and to
    // serve information through HTML pages. We activate the service to
    // provide data files only if we do really need it (swarm services)
    HttpServer * httpSrv = new HttpServer("httpSrv", "localhost:8080", true);

    for (;;) {}

    return 0;
}
