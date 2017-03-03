// -*- C++ -*-

#ifndef MESSAGE_H
#define MESSAGE_H

#include "json/json.h"

typedef Json::Value  json;

class JValue {
public:
    JValue() {}
    JValue(json val) { value = val; }
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
    json & val() { return value; }
private:
    json value;
};

class Message {

public:
    Message(json val) : value(val) {}
    Message(std::string content = std::string()) {
        Json::Reader reader;
        json val;
        reader.parse(content, val);
        value = JValue(val);
    }
    void setHeader(std::string msgId,
                   std::string msgType,
                   int msgVersion,
                   std::string source,
                   std::string target,
                   std::string dateCreation,
                   std::string dateTransmission,
                   std::string dateReception,
                   int crc,
                   std::vector<std::string> path) {
        json data;
        data["id"           ] = msgId;
        data["type"         ] = msgType;
        data["version"      ] = msgVersion;
        data["source"          ] = source;
        data["target"          ] = target;
        data["dateCreation"    ] = dateCreation;
        data["dateTransmission"] = dateTransmission;
        data["dateReception"   ] = dateReception;
        data["crc"             ] = crc;
        json pathValue(Json::arrayValue);
        for (int i = 0; i < path.size(); ++i) { pathValue[i] = path.at(i); }
        data["path"            ] = pathValue;
        value["header"] = data;
    }
    void setData(json val) { value["data"] = val; }
    void seth(std::string key, json val) {
        value["header"][key.c_str()] = val;
    }
    std::string id() { return value["header"]["id"].asString(); }
    std::string type() { return value["header"]["type"].asString(); }
    std::string version() { return value["header"]["version"].asString(); }
    std::string source() { return value["header"]["source"].asString(); }
    std::string target() { return value["header"]["target"].asString(); }
    void newTargetIs(std::string newTarget) {
        std::string lastSource = value["header"]["source"].asString();
        std::string lastTarget = value["header"]["target"].asString();
        value["header"]["path"].append(lastSource);
        value["header"]["source"] = lastTarget;
        value["header"]["target"] = newTarget;
    }
    std::vector<std::string> path() {
        std::vector<std::string> v;
        json jpth = value["header"]["path"];
        Json::ValueIterator it = jpth.begin();
        while (it != jpth.end()) { v.push_back((*it).asString()); ++it; }
        return v;
    }
    void set(std::string key, json val) {
        value["data"][key.c_str()] = val;
    }
    void set(std::string key, std::string s) {
        value["data"][key.c_str()] = s;
    }
    void set(std::string key, const char * s) {
        value["data"][key.c_str()] = s;
    }
    void set(std::string key, int n) {
        value["data"][key.c_str()] = n;
    }
    void set(std::string key, double x) {
        value["data"][key.c_str()] = x;
    }
    JValue operator()(std::string key) { return value(key); }
    JValue hdr() { return value("header"); }
    JValue data() { return value("data"); }
    std::string hdrStr() { return value("header").str(); }
    std::string dataStr() { return value("data").str(); }
    json & operator[](std::string key) { return value[key]; }
    json & jhdr() { return value["header"]; }
    json & jdata() { return value["data"]; }
    std::string str() { return value.str(); }
private:
    JValue value;
};

#endif
