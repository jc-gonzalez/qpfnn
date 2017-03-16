/******************************************************************************
 * File:    datatypes.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.datatypes
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
 *   Declare Msgdatatypes class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   CommNode
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

#ifndef DATATYPES_H
#define DATATYPES_H

//============================================================
// Group: External Dependencies
//============================================================
#include <iostream>

//------------------------------------------------------------
// Topic: System headers
//   - string
//   - map
//   - iostream
//------------------------------------------------------------

#include <string>
#include <map>
#include <set>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - json/json.h
//------------------------------------------------------------
#include "json/json.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//============================================================
// Group: Constants
//============================================================

//------------------------------------------------------------
// Topic: Spaces for product files
//------------------------------------------------------------

const std::string ExternalSpace("EXTERNAL");
const std::string UserSpace("USER");
const std::string InboxSpace("INBOX");
const std::string LocalArchSpace("LOCAL_ARCHIVE");
const std::string GatewaySpace("GATEWAY");
const std::string ProcessingSpace("PROCESSING");
const std::string ReprocessingSpace("REPROCESSING");

const std::set<std::string> URLSpaces
      { ExternalSpace,
        UserSpace,
        InboxSpace,
        LocalArchSpace,
        GatewaySpace,
        ProcessingSpace };

//============================================================
// Group: JSON based macros and classes
//============================================================

#include "json/json.h"

typedef Json::Value  json;

#define JSTR(e)  std::string e () { return value[ #e ].asString(); }
#define JINT(e)  int         e () { return value[ #e ].asInt(); }
#define JDLB(e)  double      e () { return value[ #e ].asDouble(); }
#define JBOOL(e) bool        e () { return value[ #e ].asBool(); }

#define JSTRIDX(e) std::string e (int i) { return value[i][ #e ].asString(); }
#define JINTIDX(e) int         e (int i) { return value[i][ #e ].asInt(); }
#define JDLBIDX(e) double      e (int i) { return value[i][ #e ].asDouble(); }

#define JSTRVEC(e) std::vector<std::string> e () {                      \
        json arr = value[ #e ];                                         \
        std::vector<std::string> a;                                     \
        for (int i = 0; i < arr.size(); ++i) {                          \
            a.push_back(arr[i].asString()); }                           \
        return a; }
#define JSTRINTMAP(e) std::map<std::string, int> e () {                 \
        json mp = value[ #e ];                                          \
        std::map<std::string, int> m;                                   \
        Json::Value::iterator it = mp.begin();                          \
        while (it != mp.end()) {                                        \
            m[it.key().asString()] = (int)((*it).asInt()); ++it; }      \
        return m; }
#define JSTRSTRMAP(e) std::map<std::string, std::string> e () {         \
        json mp = value[ #e ];                                          \
        std::map<std::string, std::string> m;                           \
        Json::Value::iterator it = mp.begin();                          \
        while (it != mp.end()) {                                        \
            m[it.key().asString()] = ((*it).asString()); ++it; }   \
        return m; }

#define JOBJ(e) json  e () { return value[ #e ]; }

#define JRECORD(e) JRecord e () { return JRecord(value[ #e ]); }

#define SET_CFGGRP(c,e) e = CfgGrp ## c ( v[ #e ] );
#define CFGGRP(c,e) CfgGrp ## c e;

#define SET_GRP(c,e) e = c ( v[ #e ] );
#define GRP(c,e) c e;

#define DUMPJSTR(e)  std::cerr << #e << ": " << value[ #e ].asString() << std::endl;
#define DUMPJINT(e)  std::cerr << #e << ": " << value[ #e ].asInt() << std::endl;
#define DUMPJDLB(e)  std::cerr << #e << ": " << value[ #e ].asDouble() << std::endl;
#define DUMPJBOOL(e) std::cerr << #e << ": " << std::boolalpha << value[ #e ].asBool() << std::endl;

#define FOREACH(i) for (int i = 0; i < value.size(); ++i)

#define DUMPJSTRIDX(i,e) std::cerr << #e << ": " << value[i][ #e ].asString() << std::endl;
#define DUMPJINTIDX(i,e) std::cerr << #e << ": " << value[i][ #e ].asInt() << std::endl;
#define DUMPJDLBIDX(i,e) std::cerr << #e << ": " << value[i][ #e ].asDouble() << std::endl;

#define DUMPJSTRVEC(e) do {                                             \
        std::cerr << #e << ": [" << std::endl;                          \
        for (int i = 0; i < value[ #e ].size(); ++i) {                  \
            std::cerr << "\t'" << value[ #e ][i].asString() << "'\n"; }    \
        std::cerr << "]" << std::endl; } while(0)
#define DUMPJSTRINTMAP(e) do {                                          \
        json mp = value[ #e ];                                          \
        Json::Value::iterator it = mp.begin();                          \
        std::cerr << #e << ": {" << std::endl;                          \
        while (it != mp.end()) {                                        \
            std::cerr << '\t' << it.key().asString()                    \
                      << ": " << (int)((*it).asInt()) << "\n";          \
            ++it; }                                                     \
        std::cerr << "}" << std::endl; } while(0)
#define DUMPJSTRSTRMAP(e) do {                                          \
        json mp = value[ #e ];                                          \
        Json::Value::iterator it = mp.begin();                          \
        std::cerr << #e << ": {" << std::endl;                          \
        while (it != mp.end()) {                                        \
            std::cerr << '\t' << it.key().asString()                    \
                      << ": '" << ((*it).asString()) << "'\n";          \
            ++it; }                                                     \
        std::cerr << "}" << std::endl; } while(0)

class JValue {
public:
    JValue() {}
    JValue(json val) { value = val; }
    JValue(std::string s) { fromStr(s); }
    JValue operator()(std::string key) { return JValue(value[key]); }
    json & operator[](std::string key) { return value[key]; }
    std::string str() {
        if (value.isObject()) {
            Json::FastWriter w;
            return w.write(value);
        } else {
            return value.asString();
        }
    }
    void fromStr(std::string content = std::string()) {
        Json::Reader reader;
        reader.parse(content, value);
    }
    json & val() { return value; }
    int size() { return value.size(); }
protected:
    json value;
};

class JRecord : public JValue {
public:
    JRecord() {}
    JRecord(json v) : JValue(v) {}
    JRecord(std::string s) : JValue(s) {}
    virtual void dump() {}
};

//============================================================
// Group: Basic datatypes
//============================================================

//------------------------------------------------------------
// Topic: Elementary types
//------------------------------------------------------------

typedef std::string   ProductType;
typedef std::string   DateTime;
typedef std::string   DateRange;
typedef std::string   URL;
typedef std::string   URLSpace;
typedef unsigned int  ObsId;
typedef std::string   Instrument;
typedef std::string   ObsMode;
typedef unsigned int  Exposure;
typedef std::string   Creator;
typedef std::string   Mission;
typedef std::string   Origin;
typedef std::string   ProcFunction;
typedef std::string   FileType;
typedef std::string   ProductParams;
typedef std::string   ProductId;
typedef std::string   ProductVersion;
typedef std::string   ProductStatus;
typedef unsigned int  ProductSize;
typedef std::string   Signature;
typedef std::string   FileNamePart;

//------------------------------------------------------------
// Topic: Structures
//------------------------------------------------------------

struct ProductMetadata : public JRecord {
    ProductMetadata() {}
    ProductMetadata(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTR(mission);        // %M
        DUMPJSTR(startTime);      // %f
        DUMPJSTR(endTime);        // %t
        DUMPJSTR(regTime);
        DUMPJSTR(timeInterval);   // %D
        DUMPJINT(obsId);
        DUMPJSTR(instrument);     // %I
        DUMPJSTR(obsMode);
        DUMPJINT(expos);
        DUMPJSTR(creator);
        DUMPJSTR(origin);
        DUMPJSTR(procFunc);       // %F
        DUMPJSTR(fileType);
        DUMPJSTR(params);         // %P
        DUMPJSTR(productId);
        DUMPJSTR(productType);    // %T
        DUMPJSTR(productVersion); // %V
        DUMPJSTR(productStatus);
        DUMPJSTR(productSize);
        DUMPJSTR(signature);      // %S
        DUMPJSTR(dirName);
        DUMPJSTR(baseName);
        DUMPJSTR(suffix);
        DUMPJSTR(extension);
        DUMPJSTR(url);
        DUMPJSTR(urlSpace);
        DUMPJBOOL(hadNoVersion);
    }
    JSTR(mission);        // %M
    JSTR(startTime);      // %f
    JSTR(endTime);        // %t
    JSTR(regTime);
    JSTR(timeInterval);   // %D
    JINT(obsId);
    JSTR(instrument);     // %I
    JSTR(obsMode);
    JINT(expos);
    JSTR(creator);
    JSTR(origin);
    JSTR(procFunc);       // %F
    JSTR(fileType);
    JSTR(params);         // %P
    JSTR(productId);
    JSTR(productType);    // %T
    JSTR(productVersion); // %V
    JSTR(productStatus);
    JSTR(productSize);
    JSTR(signature);      // %S
    JSTR(dirName);
    JSTR(baseName);
    JSTR(suffix);
    JSTR(extension);
    JSTR(url);
    JSTR(urlSpace);
    JBOOL(hadNoVersion);
};

//typedef std::vector<ProductMetadata>           ProductList;
//typedef std::map<ProductType, ProductMetadata> ProductCollection;


struct ProductList : public JRecord {
    ProductList() {}
    ProductList(json v) : JRecord(v) {
        products.clear();
        for (int i = 0; i < value.size(); ++i) {
            products.push_back(ProductMetadata(value[i]));
        }
    }
    virtual void dump() {
        for (auto & v: products) { v.dump(); }
    }
    std::vector<ProductMetadata> products;
};

struct ProductCollection : public JRecord {
    ProductCollection() {}
    ProductCollection(json v) : JRecord(v) {
        products.clear();
        Json::Value::iterator it = value.begin();
        while (it != value.end()) {
            products[it.key().asString()] = ProductMetadata(*it);
            ++it;
        }
    }
    virtual void dump() {
        for (auto & kv: products) { kv.second.dump(); }
    }
    std::map<ProductType, ProductMetadata> products;
};

typedef std::string                            TaskName;
typedef std::string                            TaskPath;
typedef int                                    TaskExitCode;
typedef json                                   TaskData;

#define TLIST_TASK_STATUS                                       \
    T(SCHEDULED, -2),                                           \
        T(FAILED,    -1),                                       \
        T(FINISHED,   0),                                       \
        T(RUNNING,    1),                                       \
        T(PAUSED,     2),                                       \
        T(STOPPED,    3),                                       \
        T(ARCHIVED,   4),                                       \
        T(UNKNOWN_STATE, 5)

#define T(a,b) TASK_ ## a = b
enum TaskStatus { TLIST_TASK_STATUS };
#undef T

extern std::map<TaskStatus, std::string> TaskStatusName;
extern std::map<std::string, TaskStatus> TaskStatusValue;

struct TaskInfo : public JRecord {
    TaskInfo() {}
    TaskInfo(json v) : JRecord(v) {
        SET_GRP(ProductList,inputs);
        SET_GRP(ProductList,outputs);
    }
    virtual void dump() {
        DUMPJSTR(taskName);
        DUMPJSTR(taskPath);
        DUMPJSTR(taskStart);
        DUMPJSTR(taskEnd);
        DUMPJINT(taskExitCode);
        DUMPJINT(taskStatus);
        inputs.dump();
        outputs.dump();
        DUMPJSTRSTRMAP(params);
        DUMPJSTR(taskData);
    }
    JSTR(taskName);
    JSTR(taskPath);
    JSTR(taskStart);
    JSTR(taskEnd);
    JINT(taskExitCode);
    JINT(taskStatus);
    GRP(ProductList,inputs);
    GRP(ProductList,outputs);
    JSTRSTRMAP(params);
    JSTR(taskData);
};

#ifdef COMMENTED_OUT
struct ProductList : public JRecord {
    std::vector<ProductMetadata> productList;
};

struct StringList : public JsonStruct {
    std::vector<std::string> items;

    virtual void toFields();
    virtual void toData();
};

struct ProductShortList : public StringList {};

typedef std::map<std::string, std::string> PList;

struct ParameterList : public JsonStruct {
    PList paramList;

    virtual void toFields();
    virtual void toData();
};


struct TaskAgentInfo : public JsonStruct {
    int         total;
    int         maxnum;
    int         running;
    int         waiting;
    int         paused;
    int         stopped;
    int         failed;
    int         finished;
    int         load1min;
    int         load5min;
    int         load15min;
    int         uptimesecs;
    std::string name;
    std::string client;
    std::string server;

    TaskAgentInfo();

    virtual void toFields();
    virtual void toData();
};

//------------------------------------------------------------
// Message datatypes
//------------------------------------------------------------

typedef int                   MessageId;
typedef int                   MessageVersion;
typedef std::string           NodeName;

struct NodeList : public StringList {};

typedef int                   CRC;

struct MessageHeader : public JsonStruct {
    MessageId      msgId;
    MessageVersion msgVersion;
    NodeName       source;
    NodeName       destination;
    NodeList       path;
    DateTime       dateCreation;
    DateTime       dateTransmission;
    DateTime       dateReception;
    CRC            crc;

    virtual void toFields();
    virtual void toData();
};

struct Message : public JsonStruct {
    MessageHeader header;
    virtual void toFields();
    virtual void toData();
};

struct Message_INDATA : public Message {
    ProductCollection    productsMetadata;

    virtual void toFields();
    virtual void toData();
};

struct Message_DATA_Exchange : public Message {
    ParameterList    variables;

    virtual void toFields();
    virtual void toData();
};

typedef Message_DATA_Exchange Message_DATA_RQST;
typedef Message_DATA_Exchange Message_DATA_INFO;

typedef Message_DATA_Exchange Message_MONIT_RQST;
typedef Message_DATA_Exchange Message_MONIT_INFO;

typedef Message_DATA_Exchange Message_CMD;

struct Message_TASK_Processing : public Message {
    TaskInfo    task;
    std::string rule;

    virtual void toFields();
    virtual void toData();
};

//typedef Message_TASK_Processing Message_TASK_PROC;
//typedef Message_TASK_Processing Message_TASK_RES;
class Message_TASK_PROC : public Message_TASK_Processing {};
class Message_TASK_RES  : public Message_TASK_Processing {};

#define TLIST_MESSAGE_TYPES                             \
    T(START),                                           \
                                     T(INDATA),         \
                                     T(DATA_RQST),      \
                                     T(DATA_INFO),      \
                                     T(MONIT_RQST),     \
                                     T(MONIT_INFO),     \
                                     T(TASK_PROC),      \
                                     T(TASK_RES),       \
                                     T(CMD),            \
                                     T(STOP),           \
                                     T(UNKNOWN)

#define T(a) MSG_ ## a ## _IDX
enum MessageTypeIdx { TLIST_MESSAGE_TYPES };
#undef T

#define T(a) MSG_ ## a
extern const std::string TLIST_MESSAGE_TYPES;
#undef T

extern const std::string MessageTypeId[];

extern const int BadMsgProcessing;

struct MessageData {
    MessageData(Message * p = nullptr);
    ~MessageData();
    void assign(Message * newPtr = nullptr);
    void reset();

    Message * msg;
};

#endif

#define UNUSED(x) (void)(x)

//}

#endif  /* DATATYPES_H */
