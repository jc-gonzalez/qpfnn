#include "tskage.h"

#include "channels.h"

TskAge::TskAge(const char * name, const char * addr, Synchronizer * s) : Component(name, addr, s)
{
}

TskAge::TskAge(std::string name, std::string addr, Synchronizer * s)  : Component(name, addr, s)
{
}

void TskAge::runEachIteration()
{
}
