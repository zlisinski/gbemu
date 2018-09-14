#pragma once

#include "gbemu.h"

class MemoryByteSubject;

class MemoryByteObserver
{
public:
    MemoryByteObserver() : subject(NULL) {}

    virtual void AttachToSubject(std::shared_ptr<MemoryByteSubject>) = 0;
    
    virtual void UpdateMemoryAddr(uint16_t addr, uint8_t value) = 0;

protected:
    std::shared_ptr<MemoryByteSubject> subject;
};