/******************************************************************************
 * File:    propdef.h
 *          This file is part of the LibComm communications library
 *
 * Domain:  LibComm.LibComm.propdef
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
 *   Macro declaration for definition of class data properties
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
 *   See <ChangeLog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#ifndef PROPDEF_H
#define PROPDEF_H

//============================================================
// Group: Public Declarations
//============================================================

//------------------------------------------------------------
// Topic: Constants
//   SET_NAMED_PROPERTIES - If defined, named property macro
//                          is defined instead of normal
//                          property macro.
//------------------------------------------------------------

#define SET_NAMED_PROPERTIES

//------------------------------------------------------------
// Topic: Macros
//   - Property(c,t,p,n) - Defines an internal variable with
//                         a getter and named setter
//   - PropertyRef(c,t,p,n) - Defines an internal variable with
//                            a getter and named setter
//------------------------------------------------------------

#ifdef SET_NAMED_PROPERTIES
# define Property(c,t,p,n)					\
    private: t p;							\
    public:							\
    inline t get ## n (void) const { return p; }			\
    inline t get ## n (void) { return p; }			\
    inline c & set ## n (t new ## n ) { p = new ## n ; return *this; }
# define PropertyRef(c,t,p,n)					\
    private: t p;							\
    public:							\
    inline t get ## n (void) const { return p; }			\
    inline t get ## n (void) { return p; }			\
    inline c & set ## n (t & new ## n ) { p = new ## n ; return *this; }
# define with(s)   s
#else
# define Property(t,p,n)					\
  private: t p;							\
  public:							\
  inline t get ## n (void) const { return p; }			\
  inline t get ## n (void) { return p; }			\
  inline void set ## n (t new ## n ) { p = new ## n ; }
# ifdef with
#  undef with
# endif
#endif

#endif // PROPDEF_H
