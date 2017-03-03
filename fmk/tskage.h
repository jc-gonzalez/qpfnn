// -*- C++ -*-

#ifndef TSKAGE_H
#define TSKAGE_H

#include "component.h"

class TskAge : public Component {
public:
    TskAge(const char * name, const char * addr = 0);
    TskAge(std::string name, std::string addr = std::string());
    virtual void runEachIteration();
};
#endif
