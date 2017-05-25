/******************************************************************************
 * File:    sm.h
 *          This file is part of the LibComm communications library
 *
 * Domain:  LibComm.LibComm.StateMachine
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
 *   Declare StateMachine class
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
 *   See <ChangeLog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - string
//   - map
//------------------------------------------------------------
#include <string>
#include <map>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - propdef.h
//------------------------------------------------------------
#include "propdef.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: LibComm
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace LibComm {

//==========================================================================
// Class: StateMachine
//==========================================================================
class StateMachine {

    Property(StateMachine, int, state, State);
    //Property(StateMachine, Log::LogLevel, invalidTransit, InvalidTransit);

public:

    //----------------------------------------------------------------------
    // Constructor: StateMachine
    //----------------------------------------------------------------------
StateMachine() : state(0) {} //, invalidTransit(Log::FATAL) {}

    //----------------------------------------------------------------------
    // Destructor: ~StateMachine
    // Virtual destructor
    //----------------------------------------------------------------------
    virtual ~StateMachine() {}

    //----------------------------------------------------------------------
    // Method: transitTo
    // Forces transit from current state to new state
    //
    // Parameter:
    //   newState - Target state identifier (code)
    //
    // Returns:
    //   True is transit to new state was possible
    //----------------------------------------------------------------------
    bool transitTo(int newState);

    //----------------------------------------------------------------------
    // Method: getStateName
    // Get name of state with provided identifier
    //
    // Parameter:
    //   aState - State identifier (code)
    //
    // Returns:
    //   Name of the corresponding state
    //----------------------------------------------------------------------
    std::string getStateName(int aState);

    //----------------------------------------------------------------------
    // Method: getStateIdx
    // Get integer index of state with provided name
    //
    // Parameter:
    //   aStateName - State name
    //
    // Returns:
    //   Integer value of the corresponding state name
    //----------------------------------------------------------------------
    int getStateIdx(std::string aStateName);

protected:

    //----------------------------------------------------------------------
    // Method: afterTransition
    // Perform post-transition tasks (default:none)
    //
    // Parameter:
    //   fromState - State before transition
    //   toState   - State after transition
    //----------------------------------------------------------------------
    virtual void afterTransition(int fromState, int toState);

    //----------------------------------------------------------------------
    // Method: defineState
    // Initializes map of states
    //
    // Parameters:
    //   aState - State code (integer)
    //   name   - State Name
    //----------------------------------------------------------------------
    virtual void defineState(int aState, std::string name);

    //----------------------------------------------------------------------
    // Method: defineValidTransition
    // Defines valid transitions from state A to B
    //
    // Parameteres:
    //   fromState - Initial state
    //   toState   - Final state of the transition
    //----------------------------------------------------------------------
    virtual void defineValidTransition(int fromState, int toState);

protected:

    //----------------------------------------------------------------------
    // Variables: Transitions and states
    //   validTransitions - Map of valid transitions
    //   stateNames       - Map between state names and codes
    //----------------------------------------------------------------------
    std::multimap<int, int> validTransitions;
    std::map<int, std::string> stateNames;
    std::map<std::string, int> stateIdx;
};

//}

#endif  /* STATEMACHINE_H */
