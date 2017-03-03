#include "tskage.h"

#include "channels.h"

TskAge::TskAge(const char * name, const char * addr) : Component(name, addr)
{
}

TskAge::TskAge(std::string name, std::string addr)  : Component(name, addr)
{
}

void TskAge::runEachIteration()
{
}
