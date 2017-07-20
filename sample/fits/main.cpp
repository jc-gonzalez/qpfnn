#include <iostream>
#include <fstream>
#include <sstream>

#include "fitsmetadatareader.h"

///////////////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[])
{
    std::string fileName(argv[1]);

    FitsMetadataReader mdr(fileName);
    MetadataInfo       md;

    if (mdr.getMetadataInfo(md)) {
        std::cerr << md.str() << "\n";
    }

    return 0;
}
