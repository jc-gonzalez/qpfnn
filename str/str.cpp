/******************************************************************************
 * File:    str.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.str
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
 *   Implement str class, wrapper around libstr functions
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

#include "str.h"

#include <iterator>
#include <iostream>
#include <cstring>
#include <libgen.h>
#include <string>
#include <locale>

#define showBacktrace()

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
namespace QPF {

//==========================================================================
// Namespace: str
// Wrapper around string related functions
//==========================================================================
namespace str {

//----------------------------------------------------------------------
// Function: split
// Splits a string into tokens separated by a delimites, and stores
// them in a vector of strings
//----------------------------------------------------------------------
std::vector<std::string> &split(const std::string &s, char delim,
                                std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

//----------------------------------------------------------------------
// Function: split
// Splits a string into tokens separated by a delimites, and stores
// them in a vector of strings
//----------------------------------------------------------------------
std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

//----------------------------------------------------------------------
// Function: join
// Joins a series of strings in a vector into a single string, with
// a separator in between two strings
//----------------------------------------------------------------------
std::string join(const std::vector<std::string>& vec, const char* sep)
{
    switch (vec.size()) {
    case 0:
        return std::string("");
        break;
    case 1:
        return vec.at(0);
        break;
    default: {
        std::stringstream res;
        std::copy(vec.begin(), vec.end() - 1,
                  std::ostream_iterator<std::string>(res, sep));
        res << *vec.rbegin();
        return res.str();
    } break;
    }
}

//----------------------------------------------------------------------
// Function: replaceAll
// Replace a with b within s
//----------------------------------------------------------------------
std::string replaceAll(std::string & s, std::string a, std::string b)
{
    size_t idx = s.find(a, 0);
    while (idx != std::string::npos) {
        s.replace(idx, a.length(), b);
        idx = s.find(a, idx + b.length());
    }
    return s;
}

//----------------------------------------------------------------------
// Function: mid
// Warpper for substr
//----------------------------------------------------------------------
std::string mid(std::string & s, int from, int howmany)
{
    try {
        return s.substr(from, howmany);
    } catch (...) {
        std::cerr << "ERROR: s:'" << s << "', from:" << from
                  << ", howmany:" << howmany << std::endl;
        showBacktrace();
        throw;
    }
}

//----------------------------------------------------------------------
// Function: mid
// Warpper for substr
//----------------------------------------------------------------------
std::string mid(std::string & s, int from)
{
    try {
        return s.substr(from);
    } catch (...) {
        std::cerr << "ERROR: s:'" << s << "', from:" << from
                  << std::endl;
        showBacktrace();
        throw;
    }
}

//----------------------------------------------------------------------
// Function: quoted
// Single-quotes its argument
//----------------------------------------------------------------------
std::string quoted(std::string s, char q)
{
    return q + s + q;
}

//----------------------------------------------------------------------
// Function: toUpper
// Converts string to upper case
//----------------------------------------------------------------------
void toUpper(std::string & s)
{
    auto & f = std::use_facet<std::ctype<char>>(std::locale());
    char * c = const_cast<char*>(s.data());
    f.toupper(c, c + s.size());
}

//----------------------------------------------------------------------
// Function: getExtension
// Returns the extension  part of filename
//----------------------------------------------------------------------
std::string getExtension(std::string fName)
{
    const char * filename = fName.c_str();
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return std::string(dot + 1);
}

//----------------------------------------------------------------------
// Function: getBaseName
// Returns the base name part of a filename
//----------------------------------------------------------------------
std::string getBaseName(char * fName)
{
    return std::string(basename(fName));
}

//----------------------------------------------------------------------
// Function: getBaseName
// Returns the base name part of a filename
//----------------------------------------------------------------------
std::string getBaseName(std::string fName)
{
    return getBaseName((char *)(fName.c_str()));
}

//----------------------------------------------------------------------
// Function: getDirName
// Returns the directory part of a filename
//----------------------------------------------------------------------
std::string getDirName(char * fName)
{
    return std::string(dirname(fName));
}

//----------------------------------------------------------------------
// Function: getDirName
// Returns the directory part of a filename
//----------------------------------------------------------------------
std::string getDirName(std::string fName)
{
    return getDirName((char*)(fName.c_str()));
}

//----------------------------------------------------------------------
// Function: tagToTimestamp
// Get date and time components from time tag
//----------------------------------------------------------------------
std::string tagToTimestamp(std::string tag, bool withDecimals)
{
    if (tag.empty()) {
        tag = "20010101T000000";
        if (withDecimals) { tag += ".0"; }
    }
    return std::string(tag.substr(0,4) + "-" +
                       tag.substr(4,2) + "-" +
                       tag.substr(6,2) + " " +
                       tag.substr(9,2) + ":" +
                       tag.substr(11,2) + ":" +
                       tag.substr(13,2) +
                       (withDecimals ? tag.substr(15,2) : ""));
}

}

}
