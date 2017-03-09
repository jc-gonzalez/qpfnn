// -*- C++ -*-

#ifndef DBG_H
#define DBG_H

#include <iostream>

#define TRACE_LEVEL     2
#define DEBUG_LEVEL     1
#define INFO_LEVEL      0

#define VERBOSITY_LEVEL TRACE_LEVEL

#if (VERBOSITY_LEVEL > INFO_LEVEL)
#  if (VERBOSITY_LEVEL > DEBUG_LEVEL)
#    define DBG(x)  do { \
        std::cerr << __FUNCTION__ << ":" << __LINE__ << ' ' << x << std::endl; \
    } while (0)
#    define TRC(x)  do {                                                  \
        std::cerr << __FUNCTION__ << ":" << __LINE__ << ' ' << x << std::endl; \
    } while (0)
#  else
#    define DBG(x)  do {                                                \
        std::cerr << __FUNCTION__ << ":" << __LINE__ << ' ' << x << std::endl; \
    } while (0)
#    define TRC(x)
#  endif
#else
#  define DBG(x)
#  define TRC(x)
#endif

#endif
