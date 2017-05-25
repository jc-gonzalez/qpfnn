/******************************************************************************
 * File:    infixeval.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  InFix.InfixEval
 *
 * Version:  1.2
 *
 * Date:    2016/01/11
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declare InFixEval class for infix expressions evaluation
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

#ifndef INFIXEVAL_H
#define INFIXEVAL_H

//------------------------------------------------------------
// Topic: System dependencies
//   - iostream
//   - sstream
//   - stack
//   - vector
//   - map
//   - string
//------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <stack>
#include <vector>
#include <map>
#include <string>

using std::istream;
using std::istringstream;
using std::map;
using std::stack;
using std::vector;
using std::string;

//------------------------------------------------------------
// Topic: Project dependencies
//   none
//------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// Namespace: InFix
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
namespace InFix {

#define                                                                 \
    TLIST_OF_ERRORS                                                     \
    T(NO_ERROR, "OK"),                                                  \
    T(ERR_INFIX_PARSE_ERROR, "Parse error!"),                           \
    T(ERR_INFIX_MISSING_OPND, "Missing operand!"),                      \
    T(ERR_INFIX_MISSING_OPS, "Missing operators!"),                     \
    T(ERR_INFIX_MISSING_OPEN_PAREN, "Missing open parenthesis!"),       \
    T(ERR_INFIX_UNBALANCED_PAREN, "Unbalanced parenthesis!"),           \
    T(ERR_INFIX_DIV_BY_ZERO, "Expression leads to division by zero")

#define T(e,m) e
enum InfixEvError { TLIST_OF_ERRORS };
#undef T

#define T(e,m) #e
extern char const * InfixEvErrorNames[];
#undef T

#define T(e,m) m
extern char const * InfixEvErrorMsgs[];
#undef T

#define ALLOW_SYMBOLIC

#ifdef ALLOW_SYMBOLIC
#define DEFINED_AS_TOKEN ,T(DEFINED_AS)
#else
#define DEFINED_AS_TOKEN
#endif

#define TOKENS \
    T(EOL),                                     \
        T(VALUE),                               \
        T(SYMBOL),                              \
        T(OPAREN),                              \
        T(CPAREN),                              \
        T(EXP),                                 \
        T(MULT),                                \
        T(DIV),                                 \
        T(PLUS),                                \
        T(MINUS),                               \
        T(LT),                                  \
        T(LE),                                  \
        T(GT),                                  \
        T(GE),                                  \
        T(EQUAL),                               \
        T(NOT_EQUAL),                           \
        T(AND),                                 \
        T(OR)                                   \
        DEFINED_AS_TOKEN

#define T(x) x
enum TokenType { TOKENS };
#undef T

template <typename NumericType>
struct Symbols {
    static map<string, NumericType> symbolMap;
};

template <typename NumericType>
class Token {
public:
    Token(TokenType tt = EOL, const NumericType & nt = 0, string s = string("_"))
        : theType(tt), theValue(nt), theSymbol(s) { }

    inline TokenType getType() const { return theType; }
    inline const NumericType & getValue() const { return theValue; }
    inline string getSymbol() const { return theSymbol; }

private:
    TokenType   theType;
    NumericType theValue;
    string      theSymbol;
};

template <typename NumericType>
class TokensList {
public:
    TokensList(istream & is) : in(is) { }
    Token<NumericType> getToken(int & status);

private:
    istream & in;
};

typedef Token<int> tokenType;

template <typename NumericType>
class Evaluator {
public:
    Evaluator(const string & s = string("")) : expressions(s) {}

    // Do the evaluation
    NumericType getValue();
    inline int getStatus() { return status; }
    inline void set(string s) { expressions = s; }
    inline void clear() { Symbols<NumericType>::symbolMap.clear(); }

private:
    stack<TokenType>   opStack;      // Operator stack for conversion
    stack<NumericType> postFixStack; // Stack for postfix machine
    stack<string>      symbolStack;  // Operator stack for conversion

    string expressions;              // Complete string to evaluate
    istringstream str;               // String stream
    int status;

    //-- Internal routines --

    // Evaluate a single expression
    NumericType eval();
    // Get top of postfix stack
    NumericType getTop();
    // Get top of symbol stack
    string getTopSymbol();
    // Process an operator
    void binaryOp(TokenType topOp);
    // Handle LastToken
    void processToken(const Token<NumericType> &lastToken);
};

// Explicit instantiation for integers
template struct Symbols<int>;
template class Evaluator<int>;

}

#include "infixeval.tcc"

#endif
