#include <iostream>
#include <fstream>
#include <sstream>

#include "metadatareader.h"

///////////////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[])
{
    std::string fileName(argv[1]);

    FitsMetadataReader mdr(fileName);
    MetadataInfo & md;

    mdr.getMetadataInfo(md);

    return 0;
}
