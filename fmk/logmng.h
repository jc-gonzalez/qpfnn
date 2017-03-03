// -*- C++ -*-

#ifndef LOGMNG_H
#define LOGMNG_H

#include "component.h"

class LogMng : public Component {
public:
    LogMng(const char * name, const char * addr = 0);
    LogMng(std::string name, std::string addr = std::string());
};
#endif
