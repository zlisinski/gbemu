#pragma once

#include "gbemu.h"
#include "AbsByteProxy.h"
#include "Memory.h"

class MemoryByteProxy : public AbsByteProxy
{
public:
    MemoryByteProxy(uint16_t index, Memory *memory) :
        AbsByteProxy(true),
        memory(memory),
        index(index)
    {}

    MemoryByteProxy(const MemoryByteProxy &copy) :
        AbsByteProxy(copy),
        memory(copy.memory),
        index(copy.index)
    {}

    virtual ~MemoryByteProxy() {}

    virtual uint8_t operator=(uint8_t value)
    {
        memory->WriteByte(index, value);
        return (*memory)[index];
    }

    virtual operator uint8_t() const
    {
        return memory->ReadByte(index);
    }

    virtual uint8_t Value() const
    {
        return memory->ReadByte(index);
    }

private:
    Memory *memory;
    uint16_t index;
};