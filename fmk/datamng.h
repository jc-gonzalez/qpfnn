// -*- C++ -*-

#ifndef DATAMNG_H
#define DATAMNG_H

#include "component.h"

class DataMng : public Component {
public:
    DataMng(const char * name, const char * addr = 0, Synchronizer * s = 0);
    DataMng(std::string name, std::string addr = std::string(), Synchronizer * s = 0);
};
#endif
