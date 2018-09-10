#pragma once

#include <memory>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#if 1
    #define DBG(...) do {printf(__VA_ARGS__);} while (0)
#else
    #define DBG(...)
#endif

const int MEM_SIZE = 0xFFFF;