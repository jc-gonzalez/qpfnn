/******************************************************************************
 * File:    infixeval.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  InFix.InfixEval
 *
 * Version:  1.1
 *
 * Date:    2016/01/11
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015,2016 Euclid SOC Team @ ESAC
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

#include <cmath>
using namespace std;

#define DEBUG_INFIX

#ifdef DEBUG_INFIX
#  define DBGI(s) std::cerr << s << std::endl << std::flush; std::cout << std::flush
#else
#  define DBGI(s)
#endif

////////////////////////////////////////////////////////////////////////////
// Namespace: InFix
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
namespace InFix {

#define T(e,m) #e
char const * InfixEvErrorNames[] = { TLIST_OF_ERRORS };
#undef T

#define T(e,m) m
char const * InfixEvErrorMsgs[] = { TLIST_OF_ERRORS };
#undef T

#define T(x) #x
const char * TokenName [] = { TOKENS };
#undef T

//----------------------------------------------------------------------
// Function: split
// Splits a string into tokens separated by a delimites, and stores
// them in a vector of strings
//----------------------------------------------------------------------
vector<string> split(const string &s, char delim)
{
    vector<string> elems;
    stringstream ss(s);
    string item("");
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

//----------------------------------------------------------------------
// Method: getToken
// Find the next token, skipping blanks, and return it.
// Print error message if input is unrecognized.
//----------------------------------------------------------------------
template <class NumericType>
Token<NumericType> TokensList<NumericType>::getToken(int & status)
{
    char ch;
    NumericType theValue;

    status = NO_ERROR;

    map<string, NumericType> & vars = Symbols<NumericType>::symbolMap;

    // Skip blanks
    while (in.get(ch) && ch == ' ') {}

    if (ch > 126) {
        status = ERR_INFIX_PARSE_ERROR;  // Parse error
        return EOL;
    }

    if (in.good() && ch != '\n' && ch != ';' && ch != '\0') {
        switch(ch) {
        case '^': return EXP;
        case '/': return DIV;
        case '*': return MULT;
        case '(': return OPAREN;
        case ')': return CPAREN;
        case '+': return PLUS;
        case '-': return MINUS;
        case '&': return AND;
        case '|': return OR;
        case '<':
            in.get(ch);
            if (ch == '=') {
                return LE;
            } else if (ch == '>') {
                return NOT_EQUAL;
            } else {
                in.putback(ch);
                return LT;
            }
        case '>':
            in.get(ch);
            if (ch == '=') {
                return GE;
            } else {
                in.putback(ch);
                return GT;
            }
        case '=':
            in.get(ch);
            if (ch == '=') {
                return EQUAL;
            } else {
                in.putback(ch);
#ifdef ALLOW_SYMBOLIC
                return DEFINED_AS;
#else
                status = ERR_INFIX_PARSE_ERROR;  // Parse error
                return EOL;
#endif
            }
        default:
#ifdef ALLOW_SYMBOLIC
            string name("");
            name.push_back(ch);
            while (in.get(ch) &&
                   (((ch >= '0') && (ch <= '9')) ||
                    ((ch >= 'a') && (ch <= 'z')) ||
                    ((ch >= 'A') && (ch <= 'Z')) ||
                    (ch == '_') || (ch == '.'))) { name.push_back(ch); }
            in.putback(ch);
            istringstream symbol(name);
            if (!(symbol >> theValue)) {
                DBGI("Name: " << name);
                typename map<string, NumericType>::iterator it = vars.find(name);
                if (it != vars.end()) {
                    theValue = vars[name];
                    DBGI("with value: " << theValue);
                    return Token<NumericType>(VALUE, theValue);
                } else {
                    return Token<NumericType>(SYMBOL, 0, name);
                    //status = ERR_INFIX_PARSE_ERROR;  // Parse error
                    //return EOL;
                }
            } else {
                DBGI("Constant: " << name);
                return Token<NumericType>(VALUE, theValue);
            }
            // if ((name.at(0) < '0') || (name.at(0) > '9')) {

            // } else {
            // }
#else
            in.putback(ch);
            if (!(in >> theValue)) {
                status = ERR_INFIX_PARSE_ERROR;  // Parse error
                return EOL;
            }
#endif
        }
    }

    return EOL;
}

// PREC_TABLE matches order of Token enumeration
struct Precedence {
    int inputSymbol;
    int topOfStack;
} PREC_TABLE [] = {
    { 0, -1 }, { 0, 0 },       // EOL, VALUE
    { 101, 102 },                  // SYMBOL
    { 100, 0 }, { 0, 99 },     // OPAREN, CPAREN
    { 60, 50 },                  // EXP
    { 30, 40 }, { 30, 40 },        // MULT, DIV
    { 10, 20 }, { 10, 20 },        // PLUS, MINUS
    { 10, 20 }, { 10, 20 },        // PLUS, MINUS
    { 5, 10 }, { 5, 10 },      // LT, LE
    { 5, 10 }, { 5, 10 },      // GT, GE
    { 5, 10 }, { 5, 10 },      // EQUAL, NOT_EQUAL
    { 1, 2 }, { 1, 2 },        // AND, OR
    { 0, 0 }                   // DEFINED_AS
};

//----------------------------------------------------------------------
// Method: getValue
// Public routine that performs the evaluation.
// Examines the postfix stack to see if a single result
// is left and if so, returns it; otherwise prints error.
//----------------------------------------------------------------------
template <class NumericType>
NumericType Evaluator<NumericType>::getValue()
{
    vector<string> singleExpr = split(expressions, ';');
    NumericType theResult = NumericType(0);

    opStack.push(EOL);
    for (unsigned int i = 0; i < singleExpr.size(); ++i) {
        string & s = singleExpr.at(i);
        str.clear();
        str.str(s);
        DBGI("Evaluating expression: " << s);

        theResult = eval();

        Symbols<NumericType>::symbolMap["_"] = theResult;
    }

    typename map<string, NumericType>::iterator it = Symbols<NumericType>::symbolMap.begin();
    while (it !=Symbols<NumericType>::symbolMap.end()) {
        DBGI(it->first << " : " << it->second);
        ++it;
    }

    return theResult;
};

//----------------------------------------------------------------------
// Method: eval
// Private routine that performs the evaluation of a single
// expression.
// Concatenated expressions can use pre-defined symbols or
// _ as the symbol to refer to the value of the previous expression
//----------------------------------------------------------------------
template <class NumericType>
NumericType Evaluator<NumericType>::eval()
{
    DBGI(str.str());
    TokensList<NumericType> tok(str);
    Token<NumericType> lastToken;

    do {
        lastToken = tok.getToken(status);
        processToken(lastToken);
    } while (lastToken.getType() != EOL);

    if (postFixStack.size() < 1) {
        status = ERR_INFIX_MISSING_OPND;  // Missing operand!
        return 0;
    }

    NumericType theResult = postFixStack.top();
    postFixStack.pop();
    if (!(postFixStack.size() < 1)) {
        status = ERR_INFIX_MISSING_OPS;  // Missing operators!
    }

    return theResult;
}

//----------------------------------------------------------------------
// Method: processToken
// After token is read, use operator precedence parsing
// algorithm to process it; missing opening parentheses
// are detected here.
//----------------------------------------------------------------------
template <class NumericType>
void Evaluator<NumericType>::processToken(const Token<NumericType> & lastToken)
{
    TokenType topOp;
    TokenType lastType = lastToken.getType();

    DBGI("Processing token " << TokenName[lastType]);

    switch(lastType) {
    case VALUE:
        postFixStack.push(lastToken.getValue());
        return;

    case SYMBOL:
        symbolStack.push(lastToken.getSymbol());
        return;

    case CPAREN:
        while ((topOp = opStack.top()) != OPAREN && topOp != EOL) {
            binaryOp(topOp);
        }
        if (topOp == OPAREN) {
            opStack.pop();  // Get rid of opening parentheseis
        } else {
            status = ERR_INFIX_MISSING_OPEN_PAREN;  // Missing open parenthesis
        }
        break;

    default:    // General operator case
        while (PREC_TABLE[ lastType ].inputSymbol <=
               PREC_TABLE[ topOp = opStack.top() ].topOfStack) {
            binaryOp(topOp);
        }
        if (lastType != EOL) {
            opStack.push(lastType);
        }
        break;
    }
}

//----------------------------------------------------------------------
// Method: getTop
// top and pop the postfix machine stack; return the result.
// If the stack is empty, print an error message.
//----------------------------------------------------------------------
template <class NumericType>
NumericType Evaluator<NumericType>::getTop()
{
    if (postFixStack.size() < 1) {
        status = ERR_INFIX_MISSING_OPND;  // Missing operand
        return 0;
    }

    NumericType tmp = postFixStack.top();
    postFixStack.pop();
    return tmp;
}

//----------------------------------------------------------------------
// Method: getTopSymbol
// top and pop the symbols stack; return the result.
// If the stack is empty, print an error message.
//----------------------------------------------------------------------
template <class NumericType>
string Evaluator<NumericType>::getTopSymbol()
{
    if (symbolStack.size() < 1) {
        status = ERR_INFIX_MISSING_OPND;  // Missing operand
        return 0;
    }

    string tmp = symbolStack.top();
    symbolStack.pop();
    return tmp;
}

//----------------------------------------------------------------------
// Method: topOp
// Process an operator by taking two items off the postfix
// stack, applying the operator, and pushing the result.
// Print error if missing closing parenthesis or division by 0.
//----------------------------------------------------------------------
template <class NumericType>
void Evaluator<NumericType>::binaryOp(TokenType topOp)
{
    if (topOp == OPAREN) {
        status = ERR_INFIX_UNBALANCED_PAREN;  // Unbalanced parentheses
        opStack.pop();
        return;
    }

    NumericType rhs;
    NumericType lhs;
    string symbol;

    if (topOp == DEFINED_AS) {
        symbol = getTopSymbol();
        rhs = getTop();
    } else {
        rhs = getTop();
        lhs = getTop();
    }

    if (topOp == EXP) {
        postFixStack.push(pow(lhs, rhs));
    } else if (topOp == PLUS) {
        postFixStack.push(lhs + rhs);
    } else if (topOp == MINUS) {
        postFixStack.push(lhs - rhs);
    } else if (topOp == MULT) {
        postFixStack.push(lhs * rhs);
    } else if (topOp == DIV) {
        if (rhs != 0) {
            postFixStack.push(lhs / rhs);
        } else {
            status = ERR_INFIX_DIV_BY_ZERO;  // Division by zero
            postFixStack.push(lhs);
        }
    } else if (topOp == LT) {
        postFixStack.push(lhs < rhs);
    } else if (topOp == LE) {
        postFixStack.push(lhs <= rhs);
    } else if (topOp == GT) {
        postFixStack.push(lhs > rhs);
    } else if (topOp == GE) {
        postFixStack.push(lhs >= rhs);
    } else if (topOp == EQUAL) {
        postFixStack.push(lhs == rhs);
    } else if (topOp == NOT_EQUAL) {
        postFixStack.push(lhs != rhs);
    } else if (topOp == AND) {
        postFixStack.push(lhs && rhs);
    } else if (topOp == OR) {
        postFixStack.push(lhs || rhs);
    } else if (topOp == DEFINED_AS) {
        Symbols<NumericType>::symbolMap.insert(std::pair<string,
                                               NumericType>(symbol, rhs));
        postFixStack.push(rhs);
    }
    opStack.pop();
}

// Initialization of Symbols<int>::symbolMap
template<> map<string, int> Symbols<int>::symbolMap = map<string, int>();

}
