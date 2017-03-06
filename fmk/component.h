/******************************************************************************
 * File:    component.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.Component
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
    Component(const char * name, const char * addr = 0);

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    Component(std::string name, std::string addr = std::string());

    //----------------------------------------------------------------------
    // Method: run
    //----------------------------------------------------------------------
    virtual void run();

    //----------------------------------------------------------------------
    // Method: periodicMsgInChannel
    //----------------------------------------------------------------------
    void periodicMsgInChannel(ChannelDescriptor chnl, int period, MessageString msg);

    //----------------------------------------------------------------------
    // Method: setStep
    //----------------------------------------------------------------------
    virtual void setStep(int s);

protected:
    //----------------------------------------------------------------------
    // Method: init
    //----------------------------------------------------------------------
    virtual void init();

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

private:
    //----------------------------------------------------------------------
    // Method: step
    //----------------------------------------------------------------------
    virtual void step();

protected:
    std::map<ChannelDescriptor, std::map<int, MessageString>> periodicMsgs;
    int iteration;
    std::mutex mtxStepSize;
    int stepSize;
};

#endif
