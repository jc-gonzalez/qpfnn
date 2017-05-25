/******************************************************************************
 * File:    alert.cpp
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
 *   Implement Alert class methods
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

#include "alert.h"

#include <cassert>
#include <sstream>

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
namespace QPF {

#define T(x) #x
const char * Alert::GroupName    [] = { TLIST_GROUP };
const char * Alert::SeverityName [] = { TLIST_SEVERITY };
const char * Alert::TypeName     [] = { TLIST_TYPE };
const char * Alert::VarTypeName  [] = { TLIST_VARTYPE };
#undef T

#define __glue__(a, b)  a ## b

#define T(x) { std::string( #x ), x }
std::map<std::string, Alert::Group>    Alert::GroupIdx     = { TLIST_GROUP };
std::map<std::string, Alert::Severity> Alert::SeverityIdx  = { TLIST_SEVERITY };
std::map<std::string, Alert::Type>     Alert::TypeIdx      = { TLIST_TYPE };
std::map<std::string, Alert::VarType>  Alert::VarTypeIdx   = { TLIST_VARTYPE };
#undef T

Alert::Alert() : var(0)
{
}

Alert::Alert(TimeStampType tm, Group g, Severity s, Type t, Origin o, Messages m, VariableHdl v)
{
    setTimeStamp(tm);
    setGroup(g);
    setSeverity(s);
    setType(t);
    setOrigin(o);
    setMessages(m);
    setVar(v);
}

Alert::Alert(Group g, Severity s, Type t, Origin o, std::string m, VariableHdl v)
{
    setTimeStamp(now());
    setGroup(g);
    setSeverity(s);
    setType(t);
    setOrigin(o);
    setVar(v);

    Messages msgs;
    msgs.push_back(m);
    setMessages(msgs);
}

Alert::VariableHdl Alert::createNewVar(std::string n, int i, int ll, int ul)
{
    VariableHdl v = new Variable(n);
    v->x.i  = i;
    v->ll.i = ll;
    v->ul.i = ul;
    v->type = Int;
    return v;
}

Alert::VariableHdl Alert::createNewVar(std::string n, float i, float ll, float ul)
{
    VariableHdl v = new Variable(n);
    v->x.f  = i;
    v->ll.f = ll;
    v->ul.f = ul;
    v->type = Float;
    return v;
}

Alert::VariableHdl Alert::createNewVar(std::string n, double i, double ll, double ul)
{
    VariableHdl v = new Variable(n);
    v->x.d  = i;
    v->ll.d = ll;
    v->ul.d = ul;
    v->type = Double;
    return v;
}

std::string Alert::what() const
{
    std::stringstream ss;
    ss << messages.at(0);
    if (var != 0) {
        ss << " (" << var->name << ": ";
        switch (var->type) {
        case Int:
            ss << var->x.i << ", allowed range [" << var->ll.i << ":" << var->ul.i << "]";
            break;
        case Float:
            ss << var->x.f << ", allowed range [" << var->ll.f << ":" << var->ul.f << "]";
            break;
        case Double:
            ss << var->x.d << ", allowed range [" << var->ll.d << ":" << var->ul.d << "]";
            break;
        default:
            break;
        }
        ss << ")";
    }
    return ss.str();
}

std::string Alert::varAsTuple() const
{
    std::stringstream ss;
    ss << "ROW(";
    if (var != 0) {
        ss << " '" << var->name << "', ";
        switch (var->type) {
        case Int:
            ss << "(" << var->x.i << ", 0, 0), "
               << "(" << var->ll.i << ", 0, 0), "
               << "(" << var->ul.i << ", 0, 0), 'INT'";
            break;
        case Float:
            ss << "(0, " << var->x.f << ", 0), "
               << "(0, " << var->ll.f << ", 0), "
               << "(0, " << var->ul.f << ", 0), 'FLOAT'";
            break;
        case Double:
            ss << "(0, 0, " << var->x.d << "), "
               << "(0, 0, " << var->ll.d << "), "
               << "(0, 0, " << var->ul.d << "), 'DOUBLE'";
            break;
        default:
            break;
        }
    }
    ss << ")";
    return ss.str();
}

Alert::TimeStampType Alert::now()
{
    time_t t = time(0);
    Alert::TimeStampType ts;
    (void)localtime_r(&t, &ts);
    return ts;
}

std::string Alert::here(const char * where, int line)
{
    std::stringstream ss;
    ss << where << ":" << line;
    return ss.str();
}

std::string Alert::timeStampString() const
{
    char buf[80];
    assert(0 < strftime(buf, 80, "%Y-%m-%d %X", &timeStamp));
    return std::string(buf);
}

std::string Alert::allMessages() const
{
    std::string allMsgs(messages.at(0));
    for (unsigned int i = 1; i < messages.size(); ++i) {
        allMsgs += "\n" + messages.at(i);
    }
    return allMsgs;
}

std::string Alert::dump() const
{
    return (timeStampString() +
            " [" + GroupName[group] + "::" + SeverityName[severity] + "] " +
            TypeName[type] + " " + origin + ": " + what());
}

}
