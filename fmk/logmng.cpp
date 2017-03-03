#include "logmng.h"

#include "channels.h"

LogMng::LogMng(const char * name, const char * addr) : Component(name, addr)
{
}

LogMng::LogMng(std::string name, std::string addr)  : Component(name, addr)
{
}
