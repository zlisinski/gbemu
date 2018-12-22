#pragma once

#include "gbemu.h"

class MemoryByteSubject;

class MemoryByteObserver
{
public:
    MemoryByteObserver() : memorySubject(NULL) {}

    virtual void AttachToMemorySubject(MemoryByteSubject* subject) = 0;
    
    virtual void UpdateMemoryAddr(uint16_t addr, uint8_t value) = 0;

protected:
    MemoryByteSubject* memorySubject;
};