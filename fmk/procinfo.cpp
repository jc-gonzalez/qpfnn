/******************************************************************************
 * File:    procinfo.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.ProcInfo
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
 *   Declare ProcInfo structures
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   Component
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

#include "procinfo.h"
#include "dbg.h"
#include "datatypes.h"

#define FIELDNUM(a) (std::string("\"") + std::string(#a) +      \
                     std::string("\":") + std::to_string(a))
#define FIELDSTR(a) (std::string("\"") + std::string(#a) +              \
                     std::string("\":\"") + a + std::string("\""))
#define COMMA       std::string(", ")

void TaskStatusSpectra::sum()
{
    total = (running + scheduled + paused  +
             stopped + failed    + finished);
}

std::string TaskStatusSpectra::toJsonStr()
{
    total = (running + scheduled + paused +
             stopped + failed + finished);
    return std::string("{") + (FIELDNUM(running)   + COMMA +
                               FIELDNUM(scheduled) + COMMA +
                               FIELDNUM(paused)    + COMMA +
                               FIELDNUM(stopped)   + COMMA +
                               FIELDNUM(failed)    + COMMA +
                               FIELDNUM(finished)  + COMMA +
                               FIELDNUM(total))    + std::string("}");
}

void TaskStatusSpectra::fromStr(std::string s)
{
    JValue c(s);
    running    = c["running"].asInt();
    scheduled  = c["scheduled"].asInt();
    paused     = c["paused"].asInt();
    stopped    = c["stopped"].asInt();
    failed     = c["failed"].asInt();
    finished   = c["finished"].asInt();
    total      = c["total"].asInt();
}

std::string AgentInfo::toJsonStr()
{
    return (std::string("{") +
            FIELDSTR(name) + COMMA +
            "\"counts\": " + taskStatus.toJsonStr() + COMMA +
            "\"load\": " + std::to_string((int)(load * 100)) +
            std::string("}"));
}

void AgentInfo::fromStr(std::string s)
{
    Json::FastWriter fastWriter;
    JValue a(s);
    name = a["name"].asString();
    taskStatus.fromStr(fastWriter.write(a["counts"]));
    load = (float)(a["load"].asInt()) * 0.01;
}

std::string MasterInfo::toJsonStr()
{
    //hostInfo.update();
    std::string as("");
    return (std::string("{") +
            FIELDSTR(name) + COMMA +
            "\"hostInfo\": " + hostInfo.toJsonStr() +
            std::string("}"));
}

void MasterInfo::fromStr(std::string s)
{
    Json::FastWriter fastWriter;
    JValue ph(s);
    name = ph["name"].asString();
    hostInfo.fromStr(fastWriter.write(ph["hostInfo"]));
}

std::string ProcessingHostInfo::toJsonStr()
{
    std::string as("");
    numAgents = agInfo.size();
    if (numAgents > 0) {
        as = "\"" + agInfo.at(0).name + "\": " +
            agInfo.at(0).toJsonStr();
        for (int i = 1; i < numAgents; ++i) {
            as += COMMA + "\"" + agInfo.at(i).name + "\": " +
                agInfo.at(i).toJsonStr();
        }
    }
    //hostInfo.update();
    return (std::string("{") +
            FIELDSTR(name) + COMMA +
            "\"hostInfo\": " + hostInfo.toJsonStr() + COMMA +
            FIELDNUM(numAgents) + COMMA +
            "\"agentsInfo\": {" + as + "}" + COMMA +
            FIELDNUM(numTasks) + std::string("}"));
}

void ProcessingHostInfo::fromStr(std::string s)
{
    Json::FastWriter fastWriter;
    JValue ph(s);
    name = ph["name"].asString();
    numAgents = ph["numAgents"].asInt();
    numTasks = ph["numTasks"].asInt();
    hostInfo.fromStr(fastWriter.write(ph["hostInfo"]));
    agInfo.clear();
    for (Json::ValueIterator itr = ph["agentsInfo"].begin();
         itr != ph["agentsInfo"].end(); ++itr) {
        AgentInfo ag;
        ag.fromStr(fastWriter.write(*itr));
        agInfo.push_back(ag);
    }
}

std::string SwarmInfo::toJsonStr()
{
    //hostInfo.update();
    return (std::string("{") +
            FIELDSTR(name) + COMMA +
            FIELDSTR(ip) + COMMA +
            FIELDNUM(scale) + COMMA +
            "\"hostInfo\": " + hostInfo.toJsonStr() + COMMA +
            "\"counts\": " + taskStatus.toJsonStr() + std::string("}"));
}

void SwarmInfo::fromStr(std::string s)
{
    Json::FastWriter fastWriter;
    JValue sw(s);
    name  = sw["name"].asString();
    ip    = sw["ip"].asString();
    scale = sw["scale"].asInt();
    hostInfo.fromStr(fastWriter.write(sw["hostInfo"]));
    taskStatus.fromStr(fastWriter.write(sw["counts"]));
}

std::string ProcessingFrameworkInfo::toJsonStr()
{
    std::string as, ass;
    if (hostsInfo.begin() != hostsInfo.end()) {
        std::map<std::string,
                 ProcessingHostInfo*>::iterator it = hostsInfo.begin();
        as = "\"" + it->first + "\": " + it->second->toJsonStr();
        ++it;
        while (it != hostsInfo.end()) {
            as += COMMA + "\"" + it->first + "\": " + it->second->toJsonStr();
            ++it;
        }
    }
    if (swarmInfo.begin() != swarmInfo.end()) {
        std::map<std::string,
                 SwarmInfo*>::iterator itt = swarmInfo.begin();
        ass = "\"" + itt->first + "\": " + itt->second->toJsonStr();
        ++itt;
        while (itt != swarmInfo.end()) {
            ass += COMMA + "\"" + itt->first + "\": " + itt->second->toJsonStr();
            ++itt;
        }
    }
    return (std::string("{") +
            "\"masterInfo\": " + masterInfo.toJsonStr() + COMMA +
            "\"hostsInfo\": {" + as + "}" + COMMA +
            "\"swarmInfo\": {" + ass + "}" + COMMA +
            FIELDNUM(numSrvTasks) + COMMA +
            FIELDNUM(numContTasks) + std::string("}"));
}

void ProcessingFrameworkInfo::fromStr(std::string s)
{
    Json::FastWriter fastWriter;
    JValue pf(s);
    numSrvTasks  = pf["numSrvTasks"].asInt();
    numContTasks = pf["numContTasks"].asInt();
    masterInfo.fromStr(fastWriter.write(pf["masterInfo"]));
    hostsInfo.clear();
    for (Json::ValueIterator itr = pf["hostsInfo"].begin();
         itr != pf["hostsInfo"].end(); ++itr) {
        std::string key = itr.key().asString();
        std::map<std::string, ProcessingHostInfo*>::iterator it = hostsInfo.find(key);
        ProcessingHostInfo * ph = 0;
        TRC(fastWriter.write(*itr));
        // if (it != hostsInfo.end()) {
        //     ph = it->second;
        //     ph->fromStr(fastWriter.write(*itr));
        // } else {
        ph = new ProcessingHostInfo;
        ph->fromStr(fastWriter.write(*itr));
        hostsInfo[key] = ph;
        // }
    }
    swarmInfo.clear();
    for (Json::ValueIterator itr = pf["swarmInfo"].begin();
         itr != pf["swarmInfo"].end(); ++itr) {
        std::string key = itr.key().asString();
        std::map<std::string, SwarmInfo*>::iterator it = swarmInfo.find(key);
        SwarmInfo * sw = 0;
        TRC(fastWriter.write(*itr));
        //if (it != swarmInfo.end()) {
        //    sw = it->second;
        //    sw->fromStr(fastWriter.write(*itr));
        // } else {
        sw = new SwarmInfo;
        sw->fromStr(fastWriter.write(*itr));            
        swarmInfo[key] = sw;
        //}
    }
}
