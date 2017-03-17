/******************************************************************************
 * File:    filenamespec.cpp
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
 *   Implement FileNameSpec class
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

#include "filenamespec.h"

#include <libgen.h>
#include <cstring>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iterator>
#include <algorithm>

#include "str.h"
#include "tools.h"
#include "dbg.h"

#include "euclid.h"

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

//----------------------------------------------------------------------
// Method: Constructor
//----------------------------------------------------------------------
FileNameSpec::FileNameSpec()
{
    if (reStr.empty()) { std::cerr << "PROBLEM!!" << std::endl; }
}

FileNameSpec::FileNameSpec(std::string regexp, std::string assign)
{
    setFileNameSpec(regexp, assign);
}

void FileNameSpec::setFileNameSpec(std::string regexp, std::string assign)
{
    setRegEx(regexp);
    setAssignations(assign);
    initialized = true;
}

void FileNameSpec::setRegEx(std::string regexp)
{
    reStr = regexp;

#ifdef USE_CX11_REGEX
    re = std::regex(regexp);
#else
    re = new PCRegEx(regexp);
    assert(re != 0);
#endif
}

void FileNameSpec::setAssignations(std::string assign)
{
    // Split assignations string
    // Assignation string is in the form:
    // %x=n+_+m;%y=l;...
    // where x, y, ... is one of:
    //   %M :  mission
    //   %F :  procFunction
    //   %S :  signature
    //   %I :  instrument
    //   %T :  productType
    //   %P :  params
    //   %v :  version
    //   %D :  dateRange
    //   %f :  dateStart (from)
    //   %t :  dateEnd   (to)
    // and n, m, l is the number of the sub-expression in the regular expression
    // pattern, n+m means concatenation of subexpressions n and m

    assignationsStr = assign;
    std::stringstream ss(assign);
    std::string a;
    int n;
    std::set<int> sexp;
    while (std::getline(ss, a, ';')) {
        if ((a.at(0) == '%') && (a.at(2) == '=')) {
            assignationsTpl[a.at(1)] = a.substr(3);
            str::replaceAll(assignationsTpl[a.at(1)], "+", "");
            sexp.clear();
            std::string numStr(a.substr(3));
            std::stringstream sss(str::replaceAll(numStr, "%", ""));
            numStr = "";
            while (std::getline(sss, numStr, '+')) {
                std::stringstream ssss(numStr);
                ssss >> n;
                if (n > 0) sexp.insert(n);
            }
            assignations[a.at(1)] = sexp;
        }
    }
}

void FileNameSpec::setProductIdTpl(std::string tpl)
{
    productIdTpl = tpl;
}

bool FileNameSpec::parseFileName(std::string fileName,
                                 ProductMetadata & m,
                                 std::string space,
                                 std::string creator)
{
    if (! initialized) { setFileNameSpec(reStr, assignationsStr); }

    DBG("Trying to parse '" << fileName << "' with the regex '" << reStr << "'");

    // First, get path name out of the name
    char *dirc, *basec, *bname, *dname;

    dirc = strdup(fileName.c_str());
    basec = strdup(fileName.c_str());
    dname = dirname(dirc);
    bname = basename(basec);

    std::string dirName(dname);
    std::string baseName(bname);

    // Extract the matches of the regex
#ifdef USE_CX11_REGEX
    std::cmatch mre;
    std::regex_match(baseName.c_str(), mre, re);
#else
    std::vector<std::string> mre;
    if (re->match(baseName)) {
        re->get(mre);
    } else {
        DBG("Filename does not match regex");
        return false;
    }
#endif

    // Split name in dot '.' separated sections
    std::vector<std::string> baseExt;
    str::split(baseName, '.', baseExt);

    // First one and all those starting with numbers belong to
    // the file basename, the rest is the suffix
    baseName = baseExt.at(0);
    baseExt.erase(baseExt.begin());
    while (isdigit(baseExt.at(0).at(0))) {
        baseName += "." + baseExt.at(0);
        baseExt.erase(baseExt.begin());
    }

    m["dirName"]   = dirName;
    m["baseName"]  = baseName;
    m["suffix"]    = str::join(baseExt, ".");
    m["extension"] = baseExt.at(baseExt.size() - 1);
    std::string ft = m.extension();
    str::toUpper(ft);
    m["fileType"]  = ft;

    // Extract fields according to assignations
#ifdef USE_CX11_REGEX
    unsigned int count = mre.size();
#else
    // PCRE2 returns in m[0] the whole matched string
    // substrings start at m[1]
    unsigned int count = mre.size() - 1;
#endif
    DBG("size: " << mre.size() << count);

    for (unsigned int i = 0; i < count; ++i) {
        // Extract the matches of the regex
#ifdef USE_CX11_REGEX
        int k = i;
#else
        // PCRE2 returns in mre[0] the whole matched string
        // substrings start at mre[1]
        int k = i + 1;
#endif

        for (auto & kv : assignations) {
            // indices in assignation string start with 1
            unsigned int idx = i + 1;
            std::string fld("ZZZ");
            if (idx <= count + 1) { fld = mre[k]; }
            if (kv.second.find(idx) != kv.second.end()) {
                std::string tpl(assignationsTpl[kv.first]);
                DBG(k << idx << fld << ' ' << kv.first);
                switch (kv.first) {
                case 'M':
                    m["mission"] = placeIn(m.mission(),      tpl, idx, fld);
                    break;
                case 'F':
                    m["procFunc"] = placeIn(m.procFunc(),     tpl, idx, fld);
                    break;
                case 'P':
                    m["params"] = placeIn(m.params(),       tpl, idx, fld);
                    break;
                case 'S':
                    m["signature"] = placeIn(m.signature(),    tpl, idx, fld);
                    break;
                case 'I':
                    m["instrument"] = placeIn(m.instrument(),   tpl, idx, fld);
                    break;
                case 'T':
                    m["productType"] = placeIn(m.productType(),  tpl, idx, fld);
                    break;
                case 'v':
                    if (fld.length() < 5) {
                        m["hadNoVersion"] = true;
                        fld = "01.00";
                    } else {
                        m["hadNoVersion"] = false;
                    }
                    m["productVersion"] = placeIn(m.productVersion(), tpl, idx, fld);
                    break;
                case 'D':
                    m["timeInterval"] = placeIn(m.timeInterval(), tpl, idx, fld);
                    break;
                case 'f':
                    m["startTime"] = placeIn(m.startTime(),    tpl, idx, fld);
                    break;
                case 't':
                    m["endTime"] = placeIn(m.endTime(),      tpl, idx, fld);
                    break;
                default:
                    break;
                }
            }
        }
    }

    m["productId"] = buildProductId(m);

    struct stat buf;
    if (stat(fileName.c_str(), &buf) != 0) {
        std::cerr << "PROBLEM!!" << std::endl;
    }

    decodeSignature(m);

    m["creator"]        = creator;
    m["productStatus"]  = "OK";
    m["productSize"]    = (int)(buf.st_size);
    m["regTime"]        = timeTag();
    m["url"]            = "file://" + fileName;
    m["urlSpace"]       = space;

    DBG("Signature: '" << m.signature() << "'");

    return (m.mission() == Euclid::MissionAcronym[Euclid::EUC_Mission]);
}

std::string FileNameSpec::placeIn(std::string elem, std::string & tpl, int n,
                                  std::string & fld)
{
    if (elem.empty()) { elem = tpl; }
    char intAsChar[3];
    sprintf(intAsChar, "%d", n);
    return str::replaceAll(elem, "%" + std::string(intAsChar), fld);
}

void FileNameSpec::decodeSignature(ProductMetadata & m)
{
    std::vector<std::string> signParts;
    str::split(m.params(), '-', signParts);
    int nParts = signParts.size();
    std::string obsId, expos;

    if ((m.procFunc() == "SOC") || (m.procFunc() == "HK")) {
        // Instrument-ObsMode-ObsId-Exposure
        if (nParts < 4) { return; }
        m["origin"]      = m.procFunc();
        m["instrument"]  = signParts[0];
        m["productType"] = m.procFunc() + "_" + m.instrument();
        m["obsMode"]     = signParts[1];
        obsId            = signParts[2];
        expos            = signParts[3];
    } else if ((m.procFunc() == "SIM") || (m.procFunc() == "LE1")) {
        // Instrument-ObsMode-ObsId-Exposure
        if (nParts < 4) { return; }
        m["origin"]      = m.procFunc();
        m["instrument"]  = signParts[0];
        m["productType"] = m.procFunc() + "_" + m.instrument();
        m["obsMode"]     = signParts[1];
        obsId            = signParts[2];
        expos            = signParts[3];
    } else if (m.procFunc()  == "QLA") {
        // OrigProcFunc-Instrument-ObsMode-ObsId-Exposure
        if (nParts < 5) { return; }
        m["origin"]      = signParts[0];
        m["instrument"]  = signParts[1];
        m["productType"] = m.procFunc() + "_" + m.origin() + "_" + m.instrument();
        m["obsMode"]     = signParts[2];
        obsId            = signParts[3];
        expos            = signParts[4];
    } else {
        // TBD
    }

    m["obsId"]  = (int)(strtoul(obsId.c_str(), NULL, 10));
    m["expos"]  = (int)(strtoul(expos.c_str(), NULL, 10));

    if ((m.fileType() == "LOG") || (m.fileType() == "ARCH")) {
        m["productType"] = m.productType() + "-" + m.fileType();
    }

    m["signature"] = (m.mission() + "_" +
                   m.productType() + "-" +
                   obsId + "-" + expos + "-" + m.obsMode() + "_" +
                   m.productVersion());

    m["signature"] = (obsId + "-" + expos + "-" + m.obsMode());
}

std::string FileNameSpec::buildProductId(ProductMetadata & m)
{
    std::string id(productIdTpl);

    //   %M :  mission
    //   %S :  signature
    //   %I :  instrument
    //   %T :  productType
    //   %F :  procFunction
    //   %P :  params
    //   %v :  version
    //   %D :  dateRange
    //   %f :  dateStart (from)
    //   %t :  dateEnd   (to)

    str::replaceAll(id, "%M", m.mission());
    str::replaceAll(id, "%T", m.productType());
    str::replaceAll(id, "%F", m.procFunc());
    str::replaceAll(id, "%P", m.params());
    str::replaceAll(id, "%I", m.instrument());
    str::replaceAll(id, "%S", m.signature());
    str::replaceAll(id, "%f", m.startTime());
    str::replaceAll(id, "%t", m.endTime());
    str::replaceAll(id, "%v", m.productVersion());

    return id;
}

std::string FileNameSpec::buildVersion(int major, int minor)
{
    char v[6];
    sprintf(v, "%02d.%02d", major, minor);
    return std::string(v);
}

std::string FileNameSpec::incrMinorVersion(std::string & ver)
{
    int major, minor;
    std::stringstream ss(ver);
    char c;
    ss >> major >> c >> minor;
    char v[6];
    sprintf(v, "%02d.%02d", major, minor + 1);
    return std::string(v);
}

#ifdef USE_CX11_REGEX
std::regex                        FileNameSpec::re;
#else
PCRegEx *                         FileNameSpec::re = 0;
#endif

std::string                       FileNameSpec::reStr = "([A-Z]+)_([A-Z0-9]+)_([^_]+)_([0-9]+T[\\.0-9]+Z)[_]*([0-9]*\\.*[0-9]*)";
std::string                       FileNameSpec::assignationsStr = "%T=1+2;%I=1;%S=3;%D=4;%f=4;%v=5";
std::map< char, std::set<int> >   FileNameSpec::assignations;
std::map< char, std::string >     FileNameSpec::assignationsTpl;
std::string                       FileNameSpec::productIdTpl = "%M_%T_%S_%f_%v";

bool                              FileNameSpec::initialized = false;

//}
