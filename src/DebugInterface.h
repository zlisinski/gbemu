#pragma once

#include "gbemu.h"

class DebugInterface
{
public:
    virtual void SetMemory(uint8_t *memory) = 0;
};