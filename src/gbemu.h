#pragma once

#include <stdio.h>
#include <stdint.h>

#include "Exceptions.h"
#include "Utils.h"

extern bool debugOutput;

#if 1
    #define DBG(...) \
    do {\
        if (debugOutput)\
            printf(__VA_ARGS__);\
    } while (0)
#else
    #define DBG(...)
#endif