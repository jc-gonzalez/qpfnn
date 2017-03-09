#include "logmng.h"

#include "channels.h"

LogMng::LogMng(const char * name, const char * addr, Synchronizer * s) : Component(name, addr, s)
{
}

LogMng::LogMng(std::string name, std::string addr, Synchronizer * s)  : Component(name, addr, s)
{
}
