/******************************************************************************
 * File:    euclid.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.euclid
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
 *   Declare Euclid related constants
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   CommNode
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

#ifndef EUCLID_H
#define EUCLID_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// Namespace: Euclid
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
namespace Euclid {

//==========================================================================
// Group: Message related type definitions
//==========================================================================

//------------------------------------------------------------
// Topic: Missions
//------------------------------------------------------------

#define TLIST_PREDEF_MISSIONS                   \
    T(EUC, "EUCLID Mission"), \
    T(EXT, "Product coming from EXTernal sources"), \
    T(LSS, "LSST"), \
    T(PAN, "PAN STARS"), \
    T(VIK, "VIKING"), \
    T(ANY, "Non-ambiguous source or specific processing")

#undef T

#define T(a, b) a ## _Mission
enum Mission { TLIST_PREDEF_MISSIONS };
#undef T

#define T(a, b) {a ## _Mission, #a}
static std::map<Mission,
                std::string> MissionAcronym
                    { TLIST_PREDEF_MISSIONS };
#undef T

#define T(a, b) {a ## _Mission, b}
static std::map<Mission,
                std::string> MissionDesc
                    { TLIST_PREDEF_MISSIONS };
#undef T

#define T(a, b) {#a, b}
static std::map<std::string,
                std::string> MissionAcronym2Desc
                    { TLIST_PREDEF_MISSIONS };
#undef T

//------------------------------------------------------------
// Topic: Processing Functions
//------------------------------------------------------------

#define TLIST_PREDEF_PROCESSING_FUNCTIONS                   \
    T(SIM, "Simulated Euclid data"),                        \
    T(EXT, "External source ground based obs. data"),       \
    T(VIS, "VIS processing function"),                      \
    T(NIR, "NIR processing function"),                      \
    T(SIR, "SIR processing function"),                      \
    T(SPE, "SPE processing function"),                      \
    T(SHE, "SHE processing function"),                      \
    T(PHZ, "PHZ processing function"),                      \
    T(SED, "Templates, algorithm reference, ..."),          \
    T(LE3, "LE3 processing function"),                      \
    T(LE1, "Decompressed scientific data"),                 \
    T(DQC, "Data Quality Check"),                           \
    T(CAL, "Calibration processing function"),              \
    T(TRD, "Transient detection"),                          \
    T(SOC, "Raw TM or HKTM"),                               \
    T(QLA, "Quick-Look Analysis Results")

#undef T

#define T(a, b) a ## _ProcFunction
enum ProcFunction { TLIST_PREDEF_PROCESSING_FUNCTIONS };
#undef T

#define T(a, b) {a ## _ProcFunction, #a}
static std::map<ProcFunction,
                std::string> ProcFunctionAcronym
                    { TLIST_PREDEF_PROCESSING_FUNCTIONS };
#undef T

#define T(a, b) {a ## _ProcFunction, b}
static std::map<ProcFunction,
                std::string> ProcFunctionDesc
                    { TLIST_PREDEF_PROCESSING_FUNCTIONS };
#undef T

#define T(a, b) {#a, b}
static std::map<std::string,
                std::string> ProcFunctionAcronym2Desc
                    { TLIST_PREDEF_PROCESSING_FUNCTIONS };
#undef T

}

#endif  /* EUCLID_H */
