#pragma once

#include <memory>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Exceptions.h"

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