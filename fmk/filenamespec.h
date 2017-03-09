/******************************************************************************
 * File:    filenamespec.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.FileNameSpec
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
 *   Declare FileNameSpec class
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

#ifndef FILENAMESPEC_H
#define FILENAMESPEC_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//  - cstdio
//------------------------------------------------------------
//#include <cstdio>
#include <string>
#include <set>
#include <vector>
#include <map>

#define USE_CX11_REGEX
#ifdef USE_CX11_REGEX
#  include <regex>
#else
#  include "PCRegEx.h"
#endif

//------------------------------------------------------------
// Topic: External packages
//  none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//  none
//------------------------------------------------------------
#include "datatypes.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//==========================================================================
// Class: FileNameSpec
//==========================================================================
class FileNameSpec {

public:
    FileNameSpec();
    FileNameSpec(std::string regexp, std::string assign);

    void setFileNameSpec(std::string regexp, std::string assign);
    void setRegEx(std::string regexp);
    void setAssignations(std::string assign);
    void setProductIdTpl(std::string tpl);

    bool parseFileName(std::string fileName,
                       ProductMetadata & m,
                       std::string space = InboxSpace,
                       std::string creator = "EUCLID");

    std::string placeIn(std::string elem, std::string & tpl, int n,
                       std::string & fld);

    void decodeSignature(ProductMetadata & m);

    std::string buildProductId(ProductMetadata & m);

    std::string buildVersion(int major, int minor);

    std::string incrMinorVersion(std::string & ver);

private:

#ifdef USE_CX11_REGEX
    static std::regex                        re;
#else
    static PCRegEx *                         re;
#endif

    static std::string                       reStr;
    static std::string                       assignationsStr;
    static std::map< char, std::set<int> >   assignations;
    static std::map< char, std::string >     assignationsTpl;
    static std::string                       productIdTpl;

    static bool                              initialized;
};

//}

#endif  /* FILENAMESPEC_H */
