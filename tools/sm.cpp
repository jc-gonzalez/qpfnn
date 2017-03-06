/******************************************************************************
 * File:    sm.cpp
 *          This file is part of the LibComm communications library
 *
 * Domain:  LibComm.LibComm.StateMachine
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
 *   Implement StateMachine class
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

#include "sm.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: LibComm
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace LibComm {

//----------------------------------------------------------------------
// Method: transitTo
//----------------------------------------------------------------------
bool StateMachine::transitTo(int newState)
{
  int oldState = state;
  bool isValid = false;
  std::pair <std::multimap<int, int>::iterator,
             std::multimap<int, int>::iterator> validFrom;

  validFrom = validTransitions.equal_range(state);
  for (std::multimap<int, int>::iterator it = validFrom.first;
       it != validFrom.second; ++it) {
    if (it->second == newState) {
      isValid = true;
      setState( newState );
      break;
    }
  }

  if (!isValid) {
    if (stateNames.find(newState) != stateNames.end()) {
    //   Log::log("SYSTEM", invalidTransit,
    //            "Component tried an invalid state transition from "
    //            + stateNames[state] + " to " + stateNames[newState]);
    } else {
    //   Log::log("SYSTEM", invalidTransit,
    //            "Component tried an invalid state transition from "
    //            + stateNames[state] + " to an unknown state.");
    }
  }

  afterTransition(oldState, newState);

  return isValid;
}

//----------------------------------------------------------------------
// Method: afterTransition
//----------------------------------------------------------------------
void StateMachine::afterTransition(int fromState, int toState)
{
    (void)(fromState);
    (void)(toState);
}

//----------------------------------------------------------------------
// Method: getStateName
//----------------------------------------------------------------------
std::string StateMachine::getStateName(int aState)
{
  return stateNames[aState];
}

//----------------------------------------------------------------------
// Method: getStateIdx
//----------------------------------------------------------------------
int StateMachine::getStateIdx(std::string aStateName)
{
    return stateIdx[aStateName];
}

//----------------------------------------------------------------------
// Method: defineState
// Initializes map of states
//----------------------------------------------------------------------
void StateMachine::defineState(int aState, std::string name)
{
  stateNames[aState] = name;
  stateIdx[name]     = aState;
}

//----------------------------------------------------------------------
// Method: defineValidTransition
// Initializes map of valid transitions
//----------------------------------------------------------------------
void StateMachine::defineValidTransition(int fromState, int toState)
{
  validTransitions.insert(std::make_pair(fromState, toState));
}

//}
