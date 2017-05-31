// -*- C++ -*-

#ifndef MESSAGE_H
#define MESSAGE_H

#include "datatypes.h"

#include "dbg.h"

#include <chrono>
#include <ctime>

//==========================================================================
// Class: MsgHeader
//==========================================================================
class MsgHeader : public JRecord {
public:
    MsgHeader() {}
    MsgHeader(json v) : JRecord(v) {}
    virtual void dump() {
            DUMPJSTR(id);
            DUMPJSTR(type);
            DUMPJSTR(version);
            DUMPJSTR(source);
            DUMPJSTR(target);
            DUMPJSTR(dateCreation);
            DUMPJSTR(dateTransmission);
            DUMPJSTR(dateReception);
            DUMPJINT(rc);
            DUMPJSTRVEC(path);
    }
    JSTR(id);
    JSTR(type);
    JSTR(version);
    JSTR(source);
    JSTR(target);
    JSTR(dateCreation);
    JSTR(dateTransmission);
    JSTR(dateReception);
    JINT(rc);
    JSTRVEC(path);
};

//==========================================================================
// Class: MessageBase
//==========================================================================
class MessageBase : public JRecord {
public:
    MessageBase() {}
    MessageBase(std::string s) : JRecord(s) { init(); }
    MessageBase(json v) : JRecord(v) { init(); }
    virtual void init() {
        header = MsgHeader(value["header"]);
    }
    virtual void dump() {
        header.dump();
    }
    MsgHeader   header;
};

//==========================================================================
// Class: MsgBodyCMD
//==========================================================================
class MsgBodyCMD : public JRecord {
public:
    MsgBodyCMD() {}
    MsgBodyCMD(std::string s) : JRecord(s) {}
    MsgBodyCMD(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTR(cmd);
    }
    JSTR(cmd);
};

//==========================================================================
// Class: MsgBodyINDATA
//==========================================================================
class MsgBodyINDATA : public JRecord {
public:
    MsgBodyINDATA() {}
    MsgBodyINDATA(std::string s) : JRecord(s) {}
    MsgBodyINDATA(json v) : JRecord(v) {
        products.clear();
        json & prds = value["products"];
        for (int i = 0; i < prds.size(); ++i) {
            products.push_back(ProductMetadata(prds[i]));
        }
        TRC("MsgBodyINDATA initialized");
    }
    virtual void dump() {
        for (auto & v: products) { v.dump(); }
    }
    std::vector<ProductMetadata> products;
};

//==========================================================================
// Class: MsgBodyTSK
//==========================================================================
class MsgBodyTSK : public JRecord {
public:
    MsgBodyTSK() {}
    MsgBodyTSK(std::string s) : JRecord(s) {}
    MsgBodyTSK(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTR(info);
    }
    JSTR(info);
};

//==========================================================================
// Template Class: Message
//==========================================================================
template<class T>
class Message : public MessageBase {
public:
    Message() {}
    Message(std::string s) : MessageBase(s) { init(); }
    Message(json v) : MessageBase(v) { init(); }
    virtual void init() {
        header = MsgHeader(value["header"]);
        body   = T(value["body"]);
    }
    virtual void dump() {
        header.dump();
        body.dump();
    }
    void buildHdr(std::string id,
                  std::string type,
                  std::string version,
                  std::string source,
                  std::string target,
                  std::string dateCreation,
                  std::string dateTransmission,
                  std::string dateReception) {
        std::time_t the_time = std::time(nullptr);
        header["id"]               = id;
        header["type"]             = type;
        header["version"]          = version;
        header["source"]           = source;
        header["target"]           = target;
        header["dateCreation"]     = dateCreation;
        header["dateTransmission"] = dateTransmission;
        header["dateReception"]    = dateReception;
        header["timeStamp"]        = std::string(std::ctime(&the_time));
        value["header"] = header.val();
    }
    void buildBody(T & bdy) {
        body = bdy;
        value["body"] = bdy.val();
    }
    T body;
};

#endif
