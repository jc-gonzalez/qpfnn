#include "datamng.h"

#include "channels.h"

DataMng::DataMng(const char * name, const char * addr, Synchronizer * s) : Component(name, addr, s)
{
}

DataMng::DataMng(std::string name, std::string addr, Synchronizer * s)  : Component(name, addr, s)
{
}
