// -*- C++ -*-

#ifndef SCALPROTROLE_H
#define SCALPROTROLE_H

#include <nanomsg/nn.h>
#include "nn.hpp"

#include "err.h"
#include "dbg.h"

#include <string>
#include <queue>
#include <memory>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>

#define NN_IN  1
#define NN_OUT 2

#define NN_BUS_MASTER  NN_BUS & 0x8000

#define MAX_MESSAGE_SIZE 1024

#define WAIT_BINDING 10000

typedef std::string                  MessageString;
typedef nn::socket                   Socket;
typedef std::queue<MessageString>    MsgList;

class ScalabilityProtocolRole {
public:
    ScalabilityProtocolRole();
    ~ScalabilityProtocolRole();
    virtual void createSocket(int protocol, int domain = AF_SP);
    virtual void update();
    virtual void setMsgOut(MessageString m);
    virtual int getClass();
    virtual std::string getName();
    virtual void setName(std::string & name);
    virtual std::string getAddress();
protected:
    virtual void init(int elemCls, const char * addr) = 0;
    virtual void getIncommingMessageStrings();
    virtual void processMessageString(MessageString & m) = 0;
private:
    static int getevents(int s, int events, int timeout);
protected:
    Socket *    sck;
    MsgList     iMsgList;
    MsgList     oMsgList;
    bool        readyToGo;
    int         elemClass;
    std::string elemName;
    std::string address;
    char        buf[MAX_MESSAGE_SIZE];
    int         rc;
    int         endPoint;
    int         incMsgsMask;
};

#endif
