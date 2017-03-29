// -*- C++ -*-

#ifndef DBG_H
#define DBG_H

#include <iostream>

/*
#define TRACE_LEVEL     2
#define DEBUG_LEVEL     1
#define INFO_LEVEL      0

#define VERBOSITY_LEVEL INFO_LEVEL

#if (VERBOSITY_LEVEL > INFO_LEVEL)
#  if (VERBOSITY_LEVEL > DEBUG_LEVEL)
#    define TRC(x)  do {                                                  \
        std::cerr << __FUNCTION__ << ":" << __LINE__ << ' ' << x << std::endl; \
    } while (0)
#    define DBG(x)  TRC(x)
#    define INFO(x)  TRC(x)
#  else
#    define DBG(x)  do {                                                \
        std::cerr << __FUNCTION__ << ":" << __LINE__ << ' ' << x << std::endl; \
    } while (0)
#    define INFO(x)  do {                                                \
        std::cerr << x << std::endl; \
    } while (0)
#    define TRC(x)
#  endif
#else
#  define INFO(x)  do {                                                \
      std::cout << x << std::endl; \
   } while (0)
#  define DBG(x)
#  define TRC(x)
#endif
*/

#define TRC(x)  \
    do { if (Dbg::verbosityLevel >= Dbg::TRACE_LEVEL) {			\
	    std::cerr << __FUNCTION__ << ":" << __LINE__ << " " << x << "\n"; \
	} } while (0)
#define DBG(x)  \
    do { if (Dbg::verbosityLevel >= Dbg::DEBUG_LEVEL) {			\
	    std::cerr << __FUNCTION__ << ":" << __LINE__ << " " << x << "\n"; \
	} } while (0)
#define INFO(x)  \
    do { if (Dbg::verbosityLevel >= Dbg::INFO_LEVEL) {			\
	    std::cerr << x << "\n"; \
	} } while (0)

namespace Dbg {

    const int SILENT_LEVEL = 0;
    const int INFO_LEVEL   = 1;
    const int DEBUG_LEVEL  = 2;
    const int TRACE_LEVEL  = 3;

    extern int verbosityLevel;		 
}

#endif
