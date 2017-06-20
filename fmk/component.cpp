/******************************************************************************
 * File:    component.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.component
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
 *   Implement Component class
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
 *   See <ChangelEUCog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#include "component.h"
#include "message.h"

//#include "dbhdlpostgre.h"
//#include "except.h"

//using LibComm::Log;
//#include "tools.h"
//#include "str.h"
//using namespace LibComm;

//#include <sys/time.h>
//#include <unistd.h>
//#include <time.h>

//#define WRITE_MESSAGE_FILES

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
Component::Component(const char * name, const char * addr, Synchronizer * s)
{
    init(std::string(name), std::string(addr), s);
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
Component::Component(std::string name, std::string addr, Synchronizer * s)
{
    init(name, addr, s);
}

//----------------------------------------------------------------------
// Method: init
// Initialize the component
//----------------------------------------------------------------------
void Component::init(std::string name, std::string addr, Synchronizer * s)
{
    compName    = name;
    compAddress = addr;
    synchro     = s;

    iteration = 0;
    stepSize  = 50;

    // Every component must respond to MONIT_RQST messages (at least the
    // state might be requested)
    //canProcessMessage(MSG_MONIT_RQST_IDX);

    /*
    isPeerLogMng = isPeer("LogMng") && (selfPeer()->name != "LogMng");
    isRemote     = (!ConfigurationInfo::data().isMaster);
    session      = ConfigurationInfo::data().session;
    */
    // Define log output
    Log::setLogBaseDir(Config::PATHBase + "/log");
    Log::defineLogSystem(compName);

    // Define valid state transitions
    defineValidTransitions();

    // Transit to INITIALISED
    transitTo(INITIALISED);
    InfoMsg("New state: " + getStateName(getState()));

    TRC("Creating thread for " << compName << " in " << compAddress);
    thrId = std::thread(&Component::run, this);
    thrId.detach();
}

//----------------------------------------------------------------------
// Method: addConnection
//----------------------------------------------------------------------
void Component::addConnection(ChannelDescriptor & chnl,
                              ScalabilityProtocolRole * conct)
{
    conct->setName(compName);
    connections[chnl] = conct;
}

//----------------------------------------------------------------------
// Method: periodicMsgInChannel
//----------------------------------------------------------------------
void Component::periodicMsgInChannel(ChannelDescriptor chnl, int period, MessageString msg)
{
    periodicMsgs[chnl][period] = msg;
}

//----------------------------------------------------------------------
// Method: fromInitialisedToRunning
//----------------------------------------------------------------------
void Component::fromInitialisedToRunning()
{
    transitTo(RUNNING);
    InfoMsg("New state: " + getStateName(getState()));
    for (auto & kv: connections) {
        const ChannelDescriptor & chnl = kv.first;
        ScalabilityProtocolRole * conn = kv.second;
        std::string ch(kv.first);
        InfoMsg("Connection " + ch + " - " + conn->getName() + " @ " + conn->getAddress());
    }
}

//----------------------------------------------------------------------
// Method: fromRunningToOperational
//----------------------------------------------------------------------
void Component::fromRunningToOperational()
{
    transitTo(OPERATIONAL);
    InfoMsg("New state: " + getStateName(getState()));
}

//----------------------------------------------------------------------
// Method: fromOperationalToRunning
//----------------------------------------------------------------------
void Component::fromOperationalToRunning()
{
    InfoMsg("New state: " + getStateName(getState()));
    transitTo(OFF);
}

//----------------------------------------------------------------------
// Method: fromRunningToOff
//----------------------------------------------------------------------
void Component::fromRunningToOff()
{
    InfoMsg("New state: " + getStateName(getState()));
    InfoMsg("Ending . . . ");
    synchro->notify();
}

//----------------------------------------------------------------------
// Method: updateConnections
//----------------------------------------------------------------------
void Component::updateConnections()
{
    for (auto & kv: connections) { kv.second->update(); }
}

//----------------------------------------------------------------------
// Method: processIncommingMessages
//----------------------------------------------------------------------
void Component::processIncommingMessages()
{
    MessageString m;
    TraceMsg("Component::processIncommingmessages()");
    for (auto & kv: connections) {
        const ChannelDescriptor & chnl = kv.first;
        ScalabilityProtocolRole * conn = kv.second;
        while (conn->next(m)) {
            MessageBase msg(m);
            std::string tgt(msg.header.target());
            if ((tgt != "*") and (tgt != compName)) { continue; }
            std::string type(msg.header.type());
            DbgMsg("(FROM component.cpp:)"  + compName + " received the message [" + m + "]");
            if      (chnl == ChnlCmd)        { processCmdMsg(conn, m); }
            else if (chnl == ChnlHMICmd)     { processHMICmdMsg(conn, m); }
            else if (chnl == ChnlInData)     { processInDataMsg(conn, m); }
            else if (chnl == ChnlTskSched)   { processTskSchedMsg(conn, m); }
            else if (type == MsgTskRqst)     { processTskRqstMsg(conn, m); }
            else if (type == MsgTskProc)     { processTskProcMsg(conn, m); }
            else if (type == MsgTskRep)      { processTskRepMsg(conn, m); }
            else if (chnl == ChnlTskRepDist) { processTskRepDistMsg(conn, m); }
            else    { WarnMsg("Message from unidentified channel " + chnl); }
        }
    }
}

//----------------------------------------------------------------------
// Method: sendPeriodicMsgs
//----------------------------------------------------------------------
void Component::sendPeriodicMsgs()
{
    for (auto & kv: periodicMsgs) {
        ChannelDescriptor chnl = kv.first;
        for (auto & kkv: kv.second) {
            int period = kkv.first;
            if (((iteration + 1) % period) == 0) {
                std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
                it = connections.find(chnl);
                if (it != connections.end()) {
                    ScalabilityProtocolRole * conn = it->second;
                    MessageString & msg = kkv.second;
                    conn->setMsgOut(msg);
                }
            }
        }
    }
}

//----------------------------------------------------------------------
// Method: runEachIteration
//----------------------------------------------------------------------
void Component::runEachIteration()
{
}

//----------------------------------------------------------------------
// Method: step
//----------------------------------------------------------------------
void Component::step()
{
    // Sleep until next second
    /*
    std::time_t tt = system_clock::to_time_t(system_clock::now());
    struct std::tm * ptm = std::localtime(&tt);
    ptm->tm_sec++;
    std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));
    */
    // Sleep for 'stepSize' milliseconds
    //auto start = std::chrono::high_resolution_clock::now();
    long int msnow =
        static_cast<long int>(std::chrono::duration_cast<
                              std::chrono::milliseconds>(
                                  std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    long int ms2wait = stepSize - ((msnow + stepSize) % stepSize);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms2wait));
}

//----------------------------------------------------------------------
// Method: run
//----------------------------------------------------------------------
void Component::run()
{
    // Wait for synchronisation signal
    synchro->wait();

    // Show connections
    for (auto & kv: connections) {
        ChannelDescriptor chnl = kv.first;
        ScalabilityProtocolRole * role = kv.second;
        TraceMsg(role->getName() + " in Channel " + chnl + " with address " +
                 role->getAddress() + " - " + role->getClassName());
    }

    // State: Initialised
    // Transition to: Running
    fromInitialisedToRunning();

    // State: Initialised
    // Transition to: Running
    fromRunningToOperational();

    do {
        ++iteration;
        updateConnections();
        processIncommingMessages();
        sendPeriodicMsgs();
        runEachIteration();
        step();
    } while (getState() == OPERATIONAL);

    // State: Initialised
    // Transition to: Running
    fromOperationalToRunning();

    // State: Initialised
    // Transition to: Running
    fromRunningToOff();
}

//----------------------------------------------------------------------
// Method: setStep
//----------------------------------------------------------------------
void Component::setStep(int s)
{
    std::unique_lock<std::mutex> ulck(mtxStepSize);
    stepSize = s;
}

//----------------------------------------------------------------------
// Method: defineValidTransitions
// Define the valid state transitions for the node
//----------------------------------------------------------------------
void Component::defineValidTransitions()
{
    defineState(ERROR,        "ERROR");
    defineState(OFF,          "OFF");
    defineState(INITIALISED,  "INITIALISED");
    defineState(RUNNING,      "RUNNING");
    defineState(OPERATIONAL,  "OPERATIONAL");

    defineValidTransition(ERROR,        OFF);
    defineValidTransition(OFF,          INITIALISED);
    defineValidTransition(INITIALISED,  RUNNING);
    defineValidTransition(INITIALISED,  OFF);
    defineValidTransition(INITIALISED,  ERROR);
    defineValidTransition(RUNNING,      OPERATIONAL);
    defineValidTransition(RUNNING,      OFF);
    defineValidTransition(RUNNING,      ERROR);
    defineValidTransition(OPERATIONAL,  RUNNING);
    defineValidTransition(OPERATIONAL,  OFF);
    defineValidTransition(OPERATIONAL,  ERROR);

    setState(OFF);
}

//----------------------------------------------------------------------
// Method: afterTransition
//----------------------------------------------------------------------
void Component::afterTransition(int fromState, int toState)
{
/*
    // Save task information in task_info table
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL);

    try {
        // Check that connection with the DB is possible
        dbHdl->openConnection();
        // Store new state
        dbHdl->storeState(session, selfPeer()->name, getStateName(toState));
    } catch (RuntimeException & e) {
        ErrMsg(e.what());
        return;
    }

    // Close connection
    dbHdl->closeConnection();
*/
}

//}
