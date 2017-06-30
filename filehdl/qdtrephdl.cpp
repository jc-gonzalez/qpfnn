/******************************************************************************
 * File:    qdtrephdl.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.qdtrephdl
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
 *   Implement QDTReportHandler class
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

#include "qdtrephdl.h"

#include <fstream>
#include <iostream>

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
// namespace QPF {

//----------------------------------------------------------------------
// Method: getIssues
// Retrieves from the data the list of issues found
//----------------------------------------------------------------------
bool QDTReportHandler::getIssues(std::vector<Alert*> & issues)
{
    // Loop on all the products in the report (normally, only 1)
    Json::Value::iterator prodIt = data.begin();
    while (prodIt != data.end()) {

        Json::Value const & p = (*prodIt);
        std::string product = prodIt.key().asString();
        //std::cerr << product << '\n';

        // Loop on all the CCDs
        Json::Value::iterator ccdIt = p.begin();
        while (ccdIt != p.end()) {

            Json::Value const & c = (*ccdIt);
            std::string ccdSet = ccdIt.key().asString();
            //std::cerr << '\t' << ccdSet << '\n';

            if (ccdSet.compare(0, 3, "CCD") == 0) {
                // Loop on all the quadrant
                Json::Value::iterator quadIt = c.begin();
                Json::Value::iterator quadItEnd = c.end();
                quadItEnd--;
                while (quadIt != quadItEnd) {

                    Json::Value const & q = (*quadIt);
                    std::string quadrant = quadIt.key().asString();
                    //std::cerr << "\t\t" << quadrant << '\n';

                    // Loop on all the diagnostics for the quadrant
                    Json::Value::iterator diagIt = q["diagnostics"].begin();
                    while (diagIt != q["diagnostics"].end()) {

                        std::string diagnostic = diagIt.key().asString();
                        //std::cerr << "\t\t\t" << diagnostic << '\n';

                        std::string location = (product + "." + ccdSet + "." +
                                                quadrant + "." + diagnostic);

                        checkDiagnostic(diagIt, location, issues);

                        ++diagIt; // next diagnostic
                    }

                    ++quadIt; // next quadrant
                }
            }

            // Loop on all the diagnostics for the entire CCD or Detector
            Json::Value::iterator diagIt = c["diagnostics"].begin();
            while (diagIt != c["diagnostics"].end()) {

                std::string diagnostic = diagIt.key().asString();
                //std::cerr << "\t\t\t" << diagnostic << '\n';

                std::string location = (product + "." + ccdSet + "." +
                                        diagnostic);

                checkDiagnostic(diagIt, location, issues);

                ++diagIt; // next diagnostic
            }

            ++ccdIt; // next CCD
        }

        ++prodIt; // next product
    }

    return true;
}

void QDTReportHandler::checkDiagnostic(Json::Value::iterator it,
                                       std::string & location,
                                       std::vector<Alert*> & issues)
{
    Alert::Messages msgs;

    Json::Value const & d = (*it);
    std::cerr << d["outcome"].asString();
    if (d["result"]["outcome"].asString() == "Warning") {
        msgs.push_back("Messsages:");
        Json::Value::iterator mIt;
        for (auto & v : d["result"]["messages"]) {
            msgs.push_back(v.asString());
        }

        msgs.push_back("Values:");
        Json::FastWriter writer;
        msgs.push_back(writer.write(d["values"]));

        Alert * alert = new Alert(Alert::Diagnostics,
                                  Alert::Warning,
                                  Alert::Diagnostic,
                                  location,
                                  "",
                                  0);
        alert->setMessages(msgs);
        issues.push_back(alert);
    }
}

// }
