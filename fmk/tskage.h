/******************************************************************************
 * File:    tskage.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.TskAge
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
 *   Declare Tskage class
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

#ifndef TSKAGE_H
#define TSKAGE_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - thread
//------------------------------------------------------------
#include <thread>
#include <mutex>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - component.h
//------------------------------------------------------------
#include "component.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: TskAge
//==========================================================================
class TskAge : public Component {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    TskAge(const char * name, const char * addr = 0, Synchronizer * s = 0);

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    TskAge(std::string name, std::string addr = std::string(), Synchronizer * s = 0);

protected:
    //----------------------------------------------------------------------
    // Method: runEachIteration
    //----------------------------------------------------------------------
    virtual void runEachIteration();

protected:
#undef T
#define TLIST_PSTATUS T(IDLE), T(WAITING), T(PROCESSING), T(FINISHING)

#define T(x) x
    enum ProcStatus { TLIST_PSTATUS };
#undef T
    static const std::string ProcStatusName[];

protected:
    //----------------------------------------------------------------------
    // Method: processIncommingMessages
    //----------------------------------------------------------------------
    void processIncommingMessages();

    virtual void processCmdMsg(ScalabilityProtocolRole* c, MessageString & m);
    virtual void processTskProcMsg(ScalabilityProtocolRole* c, MessageString & m);

private:
    ProcStatus pStatus;
};

//}

#endif  /* TASKAGENT_H */
