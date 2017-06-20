/******************************************************************************
 * File:    evtmng.h
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

#ifndef EVTMNG_H
#define EVTMNG_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   . component.h
//   - dwatcher.h
//------------------------------------------------------------
#include "component.h"
#include "dwatcher.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

class EvtMng : public Component {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    EvtMng(const char * name, const char * addr = 0, Synchronizer * s = 0);

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    EvtMng(std::string name, std::string addr = std::string(), Synchronizer * s = 0);

    //----------------------------------------------------------------------
    // Method: fromRunningToOperational
    //----------------------------------------------------------------------
    virtual void fromRunningToOperational();

    //----------------------------------------------------------------------
    // Method: fromOperationalToRunning
    //----------------------------------------------------------------------
    virtual void fromOperationalToRunning();

    //----------------------------------------------------------------------
    // Method: runEachIteration
    //----------------------------------------------------------------------
    virtual void runEachIteration();

protected:
    //----------------------------------------------------------------------
    // Method: processHMICmdMsg
    //----------------------------------------------------------------------
    virtual void processHMICmdMsg(ScalabilityProtocolRole* c, MessageString & m);

    //----------------------------------------------------------------------
    // Method: processTskRepDistMsg
    //----------------------------------------------------------------------
    virtual void processTskRepDistMsg(ScalabilityProtocolRole* c, MessageString & m);

    //----------------------------------------------------------------------
    // Method: processHostMonMsg
    //----------------------------------------------------------------------
    virtual void processHostMonMsg(ScalabilityProtocolRole* c, MessageString & m);

private:
    DirWatcher * dw;

    std::map<std::string, json> elements;
};
#endif
