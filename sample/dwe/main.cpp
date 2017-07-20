#include <iostream>
#include "dwatcher.h"

int main(int argc, char * argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dir1> [ <dir2> [ ... ] ]" << std::endl;
        exit(1);
    }

    DirWatcher dw;
    for (int i = 1; i < argc; ++i) {
        dw.watch(std::string(argv[i]));
    }

    DirWatcher::DirWatchEvent e;

    for (;;) {
        if (dw.nextEvent(e)) {
            std::cout << e.path << "/" << e.name << (e.isDir ? " DIR " : " ") << e.mask << std::endl;
        }
    }

    return 0;
}
