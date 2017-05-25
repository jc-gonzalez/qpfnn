/******************************************************************************
 * File:    str.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.str
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
 *   Declare str class, wrapper around libstr functions
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

#ifndef STR_H
#define STR_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//  - string
//  - sstream
//------------------------------------------------------------
#include <string>
#include <sstream>
#include <vector>

//------------------------------------------------------------
// Topic: External packages
//  none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//  none
//------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Namespace: str
// Wrapper around string related functions
//==========================================================================
namespace str {

//----------------------------------------------------------------------
// Function: toStr<T>
// Converts input variable x of type T to its string representation
//
// Parameters:
//   x  - (I) Variable of type T
//
// Returns:
//   An string with the string representation of the input argument
//----------------------------------------------------------------------
template<class T>
inline std::string toStr(const T & x)
{
    std::stringstream ss;
    ss << x;
    return ss.str();
}

//----------------------------------------------------------------------
// Function: strTo<T>
// Takes the variable from the input string to its native representation
// as a variable of type T
//
// Parameters:
//   s  - (I) Input string
//
// Returns:
//   The value of the variable x of type T
//----------------------------------------------------------------------
template<class T>
inline T strTo(const std::string & s)
{
    std::stringstream ss(s);
    T x;
    ss >> x;
    return x;
}

//----------------------------------------------------------------------
// Function: split
// Splits a string into tokens separated by a delimites, and stores
// them in a vector of strings
//
// Parameters:
//   s     - (I) Original string
//   delim - (I) Tokens delimeter
//   elems - (O) The vector where the tokens will be stored
//
// Returns:
//   The vector with a set of tokens
//----------------------------------------------------------------------
std::vector<std::string> &split(const std::string &s, char delim,
				std::vector<std::string> &elems);

//----------------------------------------------------------------------
// Function: split
// Splits a string into tokens separated by a delimites, and stores
// them in a vector of strings
//
// Parameters:
//   s     - (I) Original string
//   delim - (I) Tokens delimeter
//
// Returns:
//   A vector with a set of tokens
//----------------------------------------------------------------------
std::vector<std::string> split(const std::string &s, char delim);

//----------------------------------------------------------------------
// Function: join
// Joins a series of strings in a vector into a single string, with
// a separator in between two strings
//
// Parameters:
//   vec   - (I) Original vector of strings
//   sep   - (I) Separator of strings when joining them
//
// Returns:
//   A string
//----------------------------------------------------------------------
std::string join(const std::vector<std::string>& vec, const char* sep);

//----------------------------------------------------------------------
// Function: replaceAll
// Replace a with b within s
//
// Parameters:
//   s - (IO) Original string
//   a - (I)  Substring to be searched for
//   b - (I)  Replacement substring
//
// Returns:
//   The final value of the string after replacement
//----------------------------------------------------------------------
std::string replaceAll(std::string & s, std::string a, std::string b);

//----------------------------------------------------------------------
// Function: mid
// Warpper for substr
//----------------------------------------------------------------------
std::string mid(std::string & s, int from, int howmany);

//----------------------------------------------------------------------
// Function: mid
// Warpper for substr
//----------------------------------------------------------------------
std::string mid(std::string & s, int from);

//----------------------------------------------------------------------
// Function: quoted
// Single-quotes its argument
//----------------------------------------------------------------------
std::string quoted(std::string s, char q = '\'');

//----------------------------------------------------------------------
// Function: toUpper
// Returns the string in upper case
//----------------------------------------------------------------------
void toUpper(std::string & s);

//----------------------------------------------------------------------
// Function: getExtension
// Returns the extension  part of filename
//----------------------------------------------------------------------
std::string getExtension(std::string fName);

//----------------------------------------------------------------------
// Function: getBaseName
// Returns the base name part of a filename
//----------------------------------------------------------------------
std::string getBaseName(char * fName);

//----------------------------------------------------------------------
// Function: getBaseName
// Returns the base name part of a filename
//----------------------------------------------------------------------
std::string getBaseName(std::string fName);

//----------------------------------------------------------------------
// Function: getDirName
// Returns the directory part of a filename
//----------------------------------------------------------------------
std::string getDirName(char * fName);

//----------------------------------------------------------------------
// Function: getDirName
// Returns the directory part of a filename
//----------------------------------------------------------------------
std::string getDirName(std::string fName);

//----------------------------------------------------------------------
// Function: tagToTimestamp
// Get date and time components from time tag
//----------------------------------------------------------------------
std::string tagToTimestamp(std::string tag, bool withDecimals = true);

}

//}

#endif  /* STR_H */
