/******************************************************************************
 * File:    alert.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.Component
 *
 * Version:  1.2
 *
 * Date:    2016/06/21
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declare Alert class
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

#ifndef ALERT_H
#define ALERT_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//  - ctime
//------------------------------------------------------------
#include <ctime>
#include <vector>
#include <string>
#include <map>

//------------------------------------------------------------
// Topic: External packages
//  - propref.h
//------------------------------------------------------------
#include "propdef.h"

//------------------------------------------------------------
// Topic: Project headers
//  none
//------------------------------------------------------------

#define STR(x) #x

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
namespace QPF {

//==========================================================================
// Class: Alert
//==========================================================================
class Alert {

public:
    typedef struct tm                  TimeStampType;
    typedef std::vector<std::string>   Messages;

#define TLIST_GROUP T(Undefined), T(System), T(Diagnostics)
#define TLIST_SEVERITY T(Warning), T(Error), T(Fatal)
#define TLIST_TYPE T(Resource), T(OutOfRange), T(Diagnostic)
#define TLIST_VARTYPE T(Int), T(Float), T(Double)

#define T(x) x
    enum Group    { TLIST_GROUP };
    enum Severity { TLIST_SEVERITY };
    enum Type     { TLIST_TYPE  };
    enum VarType  { TLIST_VARTYPE };
#undef T

    static const char * GroupName [];
    static const char * SeverityName [];
    static const char * TypeName [];
    static const char * VarTypeName [];

    static std::map<std::string, Group>    GroupIdx;
    static std::map<std::string, Severity> SeverityIdx;
    static std::map<std::string, Type>     TypeIdx;
    static std::map<std::string, VarType>  VarTypeIdx;

    union VarValue {
        int i;
        float f;
        double d;
    };

    struct Variable {
        Variable(std::string n) : name(n) {}
        std::string name;
        VarValue x;
        VarValue ll;
        VarValue ul;
        VarType  type;
    };

    typedef Variable * VariableHdl;

    typedef std::string Origin;

    Property(Alert, TimeStampType, timeStamp, TimeStamp);
    Property(Alert, Group, group, Group);
    Property(Alert, Severity, severity, Severity);
    Property(Alert, Type, type, Type);
    Property(Alert, Origin, origin, Origin);
    Property(Alert, Messages, messages, Messages);
    Property(Alert, VariableHdl, var, Var);

public:

    Alert();
    Alert(TimeStampType tm, Group g, Severity s, Type t, Origin o, Messages m, VariableHdl v = 0);
    Alert(Group g, Severity s, Type t, Origin o, std::string m, VariableHdl v = 0);

public:

    static VariableHdl createNewVar(std::string n, int i, int ll, int ul);
    static VariableHdl createNewVar(std::string n, float i, float ll, float ul);
    static VariableHdl createNewVar(std::string n, double i, double ll, double ul);
    static TimeStampType now();
    static std::string here(const char * where, int line);

    std::string what() const;
    std::string varAsTuple() const;
    std::string timeStampString() const;
    std::string allMessages() const;
    std::string dump() const;
};

}

#endif  /* ALERT_H */
