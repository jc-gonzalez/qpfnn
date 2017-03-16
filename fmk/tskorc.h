/******************************************************************************
 * File:    taskorc.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.TaskOrchestrator
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
 *   Declare askOrchestrator class
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

#ifndef TSKORC_H
#define TSKORC_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   none
//------------------------------------------------------------

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
// Class: TskOrc
//==========================================================================
class TskOrc : public Component {

public:
    struct Rule {
        std::string              name;
        std::vector<std::string> inputs;
        std::vector<std::string> outputs;
        std::string              processingElement;
        std::string              condition;
    };

    typedef std::map<Rule *, ProductList>  RuleInputs;

    struct Processor {
        std::string name;
        std::string exePath;
        std::string inPath;
        std::string outPath;
        std::string version;
        int         counter;
    };

    struct OrchestrationParameters {
        std::vector<std::string>            productTypes;
        std::vector<Rule *>                 rules;
        std::map<std::string, Processor *>  processors;
    };

    struct OrchestrationMaps {
        std::multimap<std::string, Rule *>  prodAsInput;
        std::map<Rule *, std::string>       ruleDesc;
    };

public:
    //----------------------------------------------------------------------
    // ConstructorMethod: defineOrchestrationParams
    //----------------------------------------------------------------------
    TskOrc(const char * name, const char * addr = 0, Synchronizer * s = 0);

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    TskOrc(std::string name, std::string addr = std::string(), Synchronizer * s = 0);

    //----------------------------------------------------------------------
    // Method: defineOrchestrationParams
    //----------------------------------------------------------------------
    virtual void runEachIteration();

    //----------------------------------------------------------------------
    // Method: defineOrchestrationParams
    //----------------------------------------------------------------------
    void defineOrchestrationParams();

protected:
    //----------------------------------------------------------------------
    // Method: fromRunningToOperational
    //----------------------------------------------------------------------
    void fromRunningToOperational();

protected:
    //----------------------------------------------------------------------
    // Method: processCmdMsg
    //----------------------------------------------------------------------
    void processCmdMsg(ScalabilityProtocolRole* c, MessageString & m);

    //----------------------------------------------------------------------
    // Method: processInDataMsg
    //----------------------------------------------------------------------
    void processInDataMsg(ScalabilityProtocolRole* c, MessageString & m);

private:
    //----------------------------------------------------------------------
    // Method: checkRulesForProductType
    // Check if any of the rules that involve productType as input can
    // be fired
    //----------------------------------------------------------------------
    bool checkRulesForProductType(std::string prodType,
                                  RuleInputs & ruleInputs);

    //----------------------------------------------------------------------
    // Method: sendTaskSchedMsg
    // Send a TaskProcessingMsg to the Task Manager, requesting the
    // execution of a rule
    //----------------------------------------------------------------------
    bool sendTaskSchedMsg(Rule * rule,
                          ProductList & inputs);

private:
    OrchestrationParameters  orcParams;
    OrchestrationMaps        orcMaps;

    ProductCollection        catalogue;
};

//}

#endif  /* TSKORC_H */
