// -*- C++ -*-

#ifndef MESSAGE_H
#define MESSAGE_H

#include "datatypes.h"

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
    MsgBodyINDATA(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTR(cmd);
    }
    JSTR(cmd);
};

//==========================================================================
// Class: MsgBodyMONIT
//==========================================================================
class MsgBodyMONIT : public JRecord {
public:
    MsgBodyMONIT() {}
    MsgBodyMONIT(std::string s) : JRecord(s) {}
    MsgBodyMONIT(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTR(info);
    }
    JSTR(info);
};

//==========================================================================
// Class: MsgBodyTSKSCHED
//==========================================================================
class MsgBodyTSKSCHED : public JRecord {
public:
    MsgBodyTSKSCHED() {}
    MsgBodyTSKSCHED(std::string s) : JRecord(s) {}
    MsgBodyTSKSCHED(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTR(info);
    }
    JSTR(info);
};

//==========================================================================
// Class: MsgBodyTSKMONIT
//==========================================================================
class MsgBodyTSKMONIT : public JRecord {
public:
    MsgBodyTSKMONIT() {}
    MsgBodyTSKMONIT(std::string s) : JRecord(s) {}
    MsgBodyTSKMONIT(json v) : JRecord(v) {}
    virtual void dump() {
        DUMPJSTR(info);
    }
    JSTR(info);
};

//==========================================================================
// Class: MsgBodyTSKREP
//==========================================================================
class MsgBodyTSKREP : public JRecord {
public:
    MsgBodyTSKREP() {}
    MsgBodyTSKREP(std::string s) : JRecord(s) {}
    MsgBodyTSKREP(json v) : JRecord(v) {}
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
    T body;
};

#endif
