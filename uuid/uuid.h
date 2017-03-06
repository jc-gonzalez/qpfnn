/******************************************************************************
 * File:    uuid.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.UUID
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
 *   Declare UUID class, wrapper around libuuid functions
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

#ifndef UUID_H
#define UUID_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//  - uuid
//------------------------------------------------------------
#include <string>
#include "uuid/uuid.h"

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
namespace QPF {

//==========================================================================
// Class: UUID
// Wrapper around libuuid functions
//==========================================================================
class UUID {

public:
    UUID();

    void clear();
    bool compare(const UUID & uu2);
    void copy(UUID & dst);
    void generate();
    void generate_random();
    void generate_time();
    bool generate_time_safe();
    bool is_null();
    bool parse(const char *in);
    bool parse(std::string & in);
    void unparse(char *out);
    void unparse_lower(char *out);
    void unparse_upper(char *out);
    std::string asString();
    std::string asLowerString();
    std::string asUpperString();
    time_t time(struct timeval *ret_tv);
    int type();
    int variant();
    uuid_t & uuid();

private:
    uuid_t uu;
};

}

#endif  /* UUID_H */
