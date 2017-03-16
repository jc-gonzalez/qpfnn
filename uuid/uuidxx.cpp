/******************************************************************************
 * File:    uuid.cpp
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
 *   Implement UUID class, wrapper around libuuid functions
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

#include "uuidxx.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

UUID::UUID()
{
    clear();
}

/* clear.c */
void UUID::clear()
{
    uuid_clear(this->uu);
}


/* compare.c */
bool UUID::compare(const UUID & uu2)
{
    return uuid_compare(this->uu, uu2.uu) == 0;
}


/* copy.c */
void UUID::copy(UUID & dst)
{
    uuid_copy(dst.uu, this->uu);
}


/* gen_uuid.c */
void UUID::generate()
{
    uuid_generate(this->uu);
}

void UUID::generate_random()
{
    uuid_generate_random(this->uu);
}

void UUID::generate_time()
{
    uuid_generate_time(this->uu);
}

bool UUID::generate_time_safe()
{
    return uuid_generate_time_safe(this->uu) == 0;
}


/* isnull.c */
bool UUID::is_null()
{
    return uuid_is_null(this->uu) == 1;
}


/* parse.c */
bool UUID::parse(const char *in)
{
    return uuid_parse(in, this->uu) == 0;
}

bool UUID::parse(std::string & in)
{
    return uuid_parse(in.c_str(), this->uu) == 0;
}


/* unparse.c */
void UUID::unparse(char *out)
{
    uuid_unparse(this->uu, out);
}

void UUID::unparse_lower(char *out)
{
    uuid_unparse_lower(this->uu, out);
}

void UUID::unparse_upper(char *out)
{
    uuid_unparse_upper(this->uu, out);
}

std::string UUID::asString()
{
    char uuid_str[37];
    uuid_unparse(this->uu, uuid_str);
    return std::string(uuid_str);
}

std::string UUID::asLowerString()
{
    char uuid_str[37];
    uuid_unparse_lower(this->uu, uuid_str);
    return std::string(uuid_str);
}

std::string UUID::asUpperString()
{
    char uuid_str[37];
    uuid_unparse_upper(this->uu, uuid_str);
    return std::string(uuid_str);
}


/* uuid_time.c */
time_t UUID::time(struct timeval *ret_tv)
{
    return uuid_time(this->uu, ret_tv);
}

int UUID::type()
{
    return uuid_type(this->uu);
}

int UUID::variant()
{
    return uuid_variant(this->uu);
}

uuid_t & UUID::uuid()
{
    return uu;
}

//}
