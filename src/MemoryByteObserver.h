#pragma once

#include "gbemu.h"

class MemoryByteObserver
{
public:
    MemoryByteObserver() {}
    
    virtual void UpdateMemoryAddr(uint16_t addr, uint8_t value) = 0;
};