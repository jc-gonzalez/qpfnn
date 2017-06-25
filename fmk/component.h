/******************************************************************************
 * File:    component.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.Component
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
 *   Declare Component class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   MsgTypes, CommNode
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

#ifndef COMPONENT_H
#define COMPONENT_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - chrono
//   - ctime
//------------------------------------------------------------
#include <map>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>

using std::chrono::system_clock;

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - commnode.h
//   - sm.h
//   - config.h
//   - log.h
//------------------------------------------------------------
#include "commnode.h"
#include "sm.h"
#include "config.h"
#include "channels.h"
#include "log.h"
#include "sync.h"

#ifdef LogMsg

#    undef LogMsg
#    undef TraceMsg
#    undef DbgMsg
#    undef InfoMsg
#    undef WarnMsg
#    undef ErrMsg
#    undef FatalMsg

//------------------------------------------------------------
// Topic: Macros
//   - LogMsg(l, s) - Generic macro to call logMsg method
//   - TraceMsg(s)  - Macro to call logMsg for TRACE msgs
//   - DbgMsg(s)    - Macro to call logMsg for DEBUG msgs
//   - InfoMsg(s)   - Macro to call logMsg for INFO msgs
//   - WarnMsg(s)   - Macro to call logMsg for WARNING msgs
//   - ErrMsg(s)    - Macro to call logMsg for ERROR msgs
//   - FatalMsg(s)  - Macro to call logMsg for FATAL msgs
//------------------------------------------------------------

#    define LogMsg(l,s)  Log::log(compName, l, s)
#    define TraceMsg(s)  Log::log(compName, Log::TRACE,   s)
#    define DbgMsg(s)    Log::log(compName, Log::DEBUG,   s)
#    define InfoMsg(s)   Log::log(compName, Log::INFO,    s)
#    define WarnMsg(s)   Log::log(compName, Log::WARNING, s)
#    define ErrMsg(s)    Log::log(compName, Log::ERROR,   s)
#    define FatalMsg(s)  Log::log(compName, Log::FATAL,   s)

#endif // LogMsg

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

class Component : public CommNode, public StateMachine {

protected:
    // Valid Manager states
    static const int ERROR        = -1;
    static const int OFF          =  0;
    static const int INITIALISED  =  1;
    static const int RUNNING      =  2;
    static const int OPERATIONAL  =  3;

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    Component(const char * name, const char * addr = 0, Synchronizer * s = 0);

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    Component(std::string name, std::string addr = std::string(), Synchronizer * s = 0);

    //----------------------------------------------------------------------
    // Method: run
    //----------------------------------------------------------------------
    virtual void run();

    //----------------------------------------------------------------------
    // Method: addConnection
    //----------------------------------------------------------------------
    virtual void addConnection(ChannelDescriptor & chnl,
                               ScalabilityProtocolRole * conct);

    //----------------------------------------------------------------------
    // Method: periodicMsgInChannel
    //----------------------------------------------------------------------
    void periodicMsgInChannel(ChannelDescriptor chnl, int period, MessageString msg);

    //----------------------------------------------------------------------
    // Method: setStep
    //----------------------------------------------------------------------
    virtual void setStep(int s);

    //----------------------------------------------------------------------
    // Method: getName
    //----------------------------------------------------------------------
    virtual std::string getName() { return compName; }

    //----------------------------------------------------------------------
    // Method: getAddress
    //----------------------------------------------------------------------
    virtual std::string getAddress() { return compAddress; }

protected:
    //----------------------------------------------------------------------
    // Method: fromInitialisedToRunning
    //----------------------------------------------------------------------
    virtual void fromInitialisedToRunning();

    //----------------------------------------------------------------------
    // Method: fromRunningToOperational
    //----------------------------------------------------------------------
    virtual void fromRunningToOperational();

    //----------------------------------------------------------------------
    // Method: fromOperationalToRunning
    //----------------------------------------------------------------------
    virtual void fromOperationalToRunning();

    //----------------------------------------------------------------------
    // Method: fromRunningToOff
    //----------------------------------------------------------------------
    virtual void fromRunningToOff();

    //----------------------------------------------------------------------
    // Method: fromInitialisedToRunning
    //----------------------------------------------------------------------
    virtual void updateConnections();

    //----------------------------------------------------------------------
    // Method: processIncommingMessages
    //----------------------------------------------------------------------
    virtual void processIncommingMessages();

    //----------------------------------------------------------------------
    // Method: fromInitialisedToRunning
    //----------------------------------------------------------------------
    virtual void sendPeriodicMsgs();

    //----------------------------------------------------------------------
    // Method: runEachIteration
    //----------------------------------------------------------------------
    virtual void runEachIteration();

    //----------------------------------------------------------------------
    // Method: defineValidTransitions
    // Define the valid state transitions for the node
    //----------------------------------------------------------------------
    virtual void defineValidTransitions();

    //----------------------------------------------------------------------
    // Method: afterTransition
    //----------------------------------------------------------------------
    virtual void afterTransition(int fromState, int toState);

protected:
    virtual void processCmdMsg(ScalabilityProtocolRole* c, MessageString & m) {}
    virtual void processHMICmdMsg(ScalabilityProtocolRole* c, MessageString & m) {}
    virtual void processInDataMsg(ScalabilityProtocolRole* c, MessageString & m) {}
    virtual void processTskSchedMsg(ScalabilityProtocolRole* c, MessageString & m) {}
    virtual void processTskRqstMsg(ScalabilityProtocolRole* c, MessageString & m) {}
    virtual void processTskProcMsg(ScalabilityProtocolRole* c, MessageString & m) {}
    virtual void processTskRepMsg(ScalabilityProtocolRole* c, MessageString & m) {}
    virtual void processTskRepDistMsg(ScalabilityProtocolRole* c, MessageString & m) {}
    virtual void processHostMonMsg(ScalabilityProtocolRole* c, MessageString & m) {}
    virtual void processFmkMonMsg(ScalabilityProtocolRole* c, MessageString & m) {}

private:
    //----------------------------------------------------------------------
    // Method: init
    //----------------------------------------------------------------------
    virtual void init(std::string name, std::string addr, Synchronizer * s);

    //----------------------------------------------------------------------
    // Method: step
    //----------------------------------------------------------------------
    virtual void step();

protected:
    std::map<ChannelDescriptor, ScalabilityProtocolRole*> connections;
    std::map<ChannelDescriptor, std::map<int, MessageString>> periodicMsgs;

    std::string compName;
    std::string compAddress;

    std::mutex mtxStepSize;

    std::thread thrId;

    Synchronizer * synchro;
    int iteration;
    int stepSize;
};

#endif
