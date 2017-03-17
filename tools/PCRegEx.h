#ifndef PCREGEX_H
#define PCREGEX_H

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"

#include <iostream>
#include <string>
#include <cstring>
#include <vector>

class PCRegEx {

public:
    PCRegEx() : findAll(false), re(0), match_data(0) {}
    PCRegEx(std::string r, bool glob = true) : findAll(glob), re(0), match_data(0) { set(r); }
    ~PCRegEx() { clear(); }

    void clear() {
        if (match_data != 0) {
            pcre2_match_data_free(match_data);
            match_data = 0;
        }
        if (re != 0) {
            pcre2_code_free(re);
            re = 0;
        }
    }

    void global() { findAll = true; }

    bool set(std::string r) {
        clear();

        sre = r;
        cre = (PCRE2_SPTR)(sre.c_str());

        int errornumber;
        PCRE2_SIZE erroroffset;
        re = pcre2_compile(cre, /* the pattern */
                           PCRE2_ZERO_TERMINATED,   /* indicates pattern is zero-terminated */
                           0,       /* default options */
                           &errornumber,    /* for error number */
                           &erroroffset,    /* for error offset */
                           0);   /* use default compile context */
        if (re == 0) {
            PCRE2_UCHAR buffer[256];
            pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
            std::cerr << "ERROR: PCRE2 compilation failed at offset " << erroroffset
                      << ": " << buffer << std::endl;
            return false;
        }
        return true;
    }

    bool match(std::string s) {
        if (re == 0) {
            std::cerr << "ERROR: Reg.ex. is not compiled!" << std::endl;
            return false;
        }
        sstr = s;
        cstr = (PCRE2_SPTR)(sstr.c_str());
        size_t len = s.length();

        match_data = pcre2_match_data_create_from_pattern(re, 0);
        rc = pcre2_match(re,        /* the compiled pattern */
                         cstr,   /* the subject string */
                         len,    /* the length of the subject */
                         0,         /* start at offset 0 in the subject */
                         0,         /* default options */
                         match_data,        /* block for storing the result */
                         0);     /* use default match context */

        /* Matching failed: handle error cases */
        if (rc < 0) {
            if (rc != PCRE2_ERROR_NOMATCH) {
                std::cerr << "ERROR: Matching error " << rc << std::endl;
                clear();
            }
            return false;
        }

        return true;
    }

    void get(std::vector<std::string> & v) {
        PCRE2_SIZE * ovector = pcre2_get_ovector_pointer(match_data);
        size_t start;
        size_t len;
        v.clear();
        for (int i = 0; i < rc; i++) {
            //PCRE2_SPTR start = cstr + ovector[2 * i];
            start = ovector[2 * i];
            len = ovector[2 * i + 1] - ovector[2 * i];
            v.push_back(sstr.substr(start, len));
        }
    }

private:
    bool findAll;

    std::string sre;
    std::string sstr;

    pcre2_code *re;

    PCRE2_SPTR cre;         /* PCRE2_SPTR is a pointer to unsigned code units of */
    PCRE2_SPTR cstr;        /* the appropriate width (8, 16, or 32 bits). */

    pcre2_match_data * match_data;
    int rc;
};

#endif //_PCREGEXPP_H_
