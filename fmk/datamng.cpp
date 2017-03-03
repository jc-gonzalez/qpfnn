#include "datamng.h"

#include "channels.h"

DataMng::DataMng(const char * name, const char * addr) : Component(name, addr)
{
}

DataMng::DataMng(std::string name, std::string addr)  : Component(name, addr)
{
}
