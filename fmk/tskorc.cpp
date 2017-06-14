/******************************************************************************
 * File:    tskorc.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.taskorc
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
 *   Implement TaskOrchestrator class
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
 *   See <Changelog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#include "tskorc.h"

#include <sys/time.h>
#include <iterator>

#include "urlhdl.h"
#include "str.h"
#include "infixeval.h"
#include "log.h"
#include "tools.h"
#include "channels.h"
#include "message.h"
#include "config.h"

using Configuration::cfg;

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
TskOrc::TskOrc(const char * name, const char * addr, Synchronizer * s)
    : Component(name, addr, s)
{
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
TskOrc::TskOrc(std::string name, std::string addr, Synchronizer * s)
    : Component(name, addr, s)
{
}

//----------------------------------------------------------------------
// Method: defineOrchestrationParams
//----------------------------------------------------------------------
void TskOrc::defineOrchestrationParams()
{
    // Take orchestration parameters from Configuration

    // 1. Product Types
    orcParams.productTypes.clear();
    orcParams.productTypes = cfg.products.productTypes();

    // 2. Rules
    json jobj = cfg.orchestration.rules.val();
    for (int i = 0; i < jobj.size(); ++i) {
        Rule * rule = new Rule;
        std::string ipTypes = jobj[i]["inputs"].asString();
        std::string opTypes = jobj[i]["outputs"].asString();
        rule->name              = "Rule_" + str::toStr<int>(i);
        rule->inputs            = str::split(ipTypes, ',');
        rule->outputs           = str::split(opTypes, ',');
        rule->processingElement = jobj[i]["processing"].asString();
        rule->condition         = jobj[i]["condition"].asString();
        orcParams.rules.push_back(rule);
    }

    // 3. Processors
    for (auto & kv : cfg.orchestration.processors()) {
        Processor * proc = new Processor;
        proc->name = kv.second;
        orcParams.processors[kv.first] = proc;
    }

    // 4. Create map from product type to rules
    for (unsigned int i = 0; i < orcParams.rules.size(); ++i) {
        Rule * rule = orcParams.rules.at(i);
        for (unsigned int j = 0; j < rule->inputs.size(); ++j) {
            std::string inputProduct = rule->inputs.at(j);
            orcMaps.prodAsInput.insert(std::pair<std::string, Rule *>(inputProduct, rule));
        }
    }
}

//----------------------------------------------------------------------
// Method: fromRunningToOperational
//----------------------------------------------------------------------
void TskOrc::fromRunningToOperational()
{
    // Setup orchestration parameters
    defineOrchestrationParams();

    // Dump rules
    std::stringstream ss;
    for (unsigned int i = 0; i < orcParams.rules.size(); ++i) {
        Rule * rule = orcParams.rules.at(i);
        ss.str("");
        ss << rule->processingElement << " :: ";
        std::copy(rule->inputs.begin(), rule->inputs.end(),
                  std::ostream_iterator<std::string>(ss," "));
        ss << " ==[" + rule->condition + "]==> ";
        std::copy(rule->outputs.begin(), rule->outputs.end(),
                  std::ostream_iterator<std::string>(ss," "));
        InfoMsg("Orc.Rule#" + str::toStr<int>(i + 1) + ":  " + ss.str());
        orcMaps.ruleDesc[rule] = ss.str();
    }

    transitTo(OPERATIONAL);
    InfoMsg("New state: " + getStateName(getState()));
}

//----------------------------------------------------------------------
// Method: runEachIteration
//----------------------------------------------------------------------
void TskOrc::runEachIteration()
{
}

//----------------------------------------------------------------------
// Method: processCmdMsg
//----------------------------------------------------------------------
void TskOrc::processCmdMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    MessageString ack = "ACK from " + compName;
    c->setMsgOut(ack);
}

//----------------------------------------------------------------------
// Method: processInDataMsg
//----------------------------------------------------------------------
void TskOrc::processInDataMsg(ScalabilityProtocolRole* c, MessageString & m)
{
    Message<MsgBodyINDATA> msg(m);
    MsgBodyINDATA & body = msg.body;

    // Synthetic INDATA messages, that means reading products from folder
    URLHandler urlh;
    for (auto & m : msg.body.products) {
        urlh.setProduct(m);
        m = urlh.fromInbox2LocalArch(false);

        // Append product to catalogue
        std::string prodType = m.productType();
        catalogue.products[prodType] = m;

        // Check the product type as input for any rule
        RuleInputs ruleInputs;
        if (checkRulesForProductType(prodType, ruleInputs)) {
            for (auto & kv : ruleInputs) {
                InfoMsg("Product type " + prodType + " fires rule: " +
                        orcMaps.ruleDesc[kv.first]);
                for (auto & itInp : kv.second.products) {
                    InfoMsg("Input: " + itInp.productId());
                }
                // Generate and send processing task to TskMng
                InfoMsg("Sending task scheduling message");
                sendTaskSchedMsg(kv.first, kv.second);
            }
        }
    }
}

//----------------------------------------------------------------------
// Method: checkRulesForProductType
// Check if any of the rules that involve productType as input can
// be fired
//----------------------------------------------------------------------
bool TskOrc::checkRulesForProductType(std::string prodType,
                                      RuleInputs & ruleInputs)
{
    bool atLeastOneRuleFired = false;

    ruleInputs.clear();

    DbgMsg("Current catalogue contents:");
    for (auto & kv : catalogue.products) {
        ProductMetadata & m = kv.second;
        DbgMsg("  - " + kv.first + " : " + m.productId() + " [" + m.url() + "]");
    }

    // Select all the rules that involve prodType as input
    std::pair<std::multimap<std::string, Rule *>::iterator,
            std::multimap<std::string, Rule *>::iterator> range;
    range = orcMaps.prodAsInput.equal_range(prodType);

    // If no rule found for that product type, no rule can be fired
    if (range.first == range.second) { return false; }

#define EVAL_CONDITION

#ifdef EVAL_CONDITION
    // Condition evaluator
    InFix::Evaluator<int> ev;
#else
#endif

    // Loop on selected rules
    std::multimap<std::string, Rule *>::iterator it = range.first;
    for (; it != range.second; ++it) {

#ifdef EVAL_CONDITION
        // Rule condition
        std::string condStr("");
#else
#endif
        Rule * rule = (*it).second;
        std::set<ProductType> requiredInputs(rule->inputs.begin(),
                                             rule->inputs.end());
        std::set<ProductType> availableInputs;
        ProductList inputs;

        // Check if all the inputs for this rule are available in the DB
        for (unsigned int i = 0; i < rule->inputs.size(); ++i) {
            std::string prd = rule->inputs.at(i);

            // Select all the products in the catalogue with this product type
            std::pair<std::multimap<std::string, ProductMetadata>::iterator,
                    std::multimap<std::string, ProductMetadata>::iterator> rangeProd;
            rangeProd = catalogue.products.equal_range(prd);

            // Check that we have at least one product with the right product
            // type (specified by the loop)
            // Otherwise, the rule cannot be fired
            if (rangeProd.first == rangeProd.second) {
                inputs.products.clear();
                continue;
            }

            // If this input product is available, store its metadata in the
            // list for the current rule
            std::multimap<std::string, ProductMetadata>::iterator itt = rangeProd.second;
            itt--;
            ProductMetadata & m = (*itt).second;
            std::string pt = m.productType();
            inputs.products.push_back(m);
            availableInputs.insert(pt);

#ifdef EVAL_CONDITION
            // Store also its metadata fields for rule conditions in the
            // condition string
            int date1 = str::strTo<int>(m.startTime().substr(0, 8));
            int time1 = str::strTo<int>(m.startTime().substr(9, 6));
//            int date2 = str::strTo<int>(m.endTime().substr(0, 8));
//            int time2 = str::strTo<int>(m.endTime().substr(9, 6));
            condStr += pt + ".date = " + str::toStr<int>(date1) + ";" +
                       pt + ".time = " + str::toStr<int>(time1) + ";" /*+
                       pt + ".date_end = " + str::toStr<int>(date2) + ";" +
                       pt + ".time_end = " + str::toStr<int>(time2) + ";"*/;
#else
#endif
        }

        std::ostream_iterator<ProductType> out_it (std::cerr, ", ");

        std::cerr << "requiredInputs :> ";
        std::copy(requiredInputs.begin(), requiredInputs.end(), out_it);
        std::cerr << "availableInputs :> ";
        std::copy(availableInputs.begin(), availableInputs.end(), out_it);
        std::cerr << std::endl;

        if (availableInputs == requiredInputs) {
#ifdef EVAL_CONDITION
            // Evaluate the rule condition:
            int result = 0;
            if (! rule->condition.empty()) {
                // a. Include the condition in the condition string
                condStr += rule->condition;
                // b. Evaluate condition
                std::cerr << "Evaluating condition: " << condStr << std::endl;
                ev.clear();
                ev.set(condStr);
                try {
                    result = ev.getValue();
                } catch (...) {
                    result = 1;
                }
            } else {
                result = 1;
            }
            if (result > 0) {
                ruleInputs[rule] = inputs;
                atLeastOneRuleFired = true;
            }
#else
            ruleInputs[rule] = inputs;
            atLeastOneRuleFired = true;
#endif
        }

    } // check for each rule with provided product type

    return atLeastOneRuleFired;
}

//----------------------------------------------------------------------
// Method: sendTaskSchedMsg
// Send a TaskProcessingMsg to the Task Manager, requesting the
// execution of a rule
//----------------------------------------------------------------------
bool TskOrc::sendTaskSchedMsg(Rule * rule,
                              ProductList & inputs)
{
    // Define and set task object
    TaskInfo task;

    DateTime epoch = timeTag();
    task["taskName"]     = rule->name + "_" + epoch;
    task["taskPath"]     = rule->processingElement;
    task["taskStart"]    = epoch;
    task["taskEnd"]      = "";
    task["taskExitCode"] = 0;
    task["taskStatus"]   = TASK_SCHEDULED;
    task["taskSet"]      = "CONTAINER";
    task["taskSession"]  = cfg.sessionId;
    task["params"]       = nullJson;

    URLHandler urlh;
    int i = 0;
    for (auto & m : inputs.products) {
        urlh.setProduct(m);
        ProductMetadata & mg = urlh.fromLocalArch2Gateway();
        task.inputs.products.push_back(mg);
        task["inputs"][i] = mg.val();
        ++i;
    }

    i = 0;
    for (auto & s : rule->outputs) {
        ProductMetadata m;
        m["productType"] = s;
        task.outputs.products.push_back(m);
        task["outputs"][i] = m.val();
        ++i;
    }

    // Create message and send
    Message<MsgBodyTSK> msg;
    msg.buildHdr(ChnlTskSched,
                 ChnlTskSched,
                 "1.0",
                 compName,
                 "*",
                 "", "", "");

    MsgBodyTSK body;
    body["info"] = task.val();
    msg.buildBody(body);

    std::map<ChannelDescriptor, ScalabilityProtocolRole*>::iterator it;
    it = connections.find(ChnlTskSched);
    if (it != connections.end()) {
        ScalabilityProtocolRole * conn = it->second;
        conn->setMsgOut(msg.str());
        return true;
    } else {
        return false;
    }
}
