#pragma once

#include <array>

#include "gbemu.h"

const uint16_t MEM_SB = 0xFF01;
const uint16_t MEM_SC = 0xFF02;

class Memory
{
public:
    Memory();
    virtual ~Memory();

    uint8_t ReadByte(uint16_t index) const;
    uint8_t *GetBytePtr(uint16_t index); // Bypasses checking of writes to special addresses. Don't use unless you know what you are doing.
    uint8_t operator[](uint16_t index) const;

    void WriteByte(uint16_t index, uint8_t byte);

    void ClearMemory();

private:
    std::array<uint8_t, MEM_SIZE> memory;
};