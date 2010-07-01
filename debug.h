#ifndef ARCHI_DEBUG_H_
#define ARCHI_DEBUG_H_

#include "stdio.h"

#define DEBUG_ASSERT(expr) \
  do {\
    if (!(expr)) {\
      fprintf(stderr, "%s (%s:%d): Assertion '%s' failed\n", __FUNCTION__, __FILE__, __LINE__, #expr );\
      abort();\
    }\
  } while(0)

#endif
