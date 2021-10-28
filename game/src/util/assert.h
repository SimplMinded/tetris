#pragma once

#include <cstdio>
#include <cstdlib>

#define ASSERT(x) \
    if (!(x)) { \
        fprintf(stderr, "[ASSERT] %s:%d: \"%s\"\n", __FILE__, __LINE__, #x); \
        abort(); \
    }
