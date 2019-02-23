#pragma once

#include "gbemu.h"
#include "MemoryBankController.h"

class DebugInterface
{
public:
    virtual void SetMemory(uint8_t *memory) = 0;
    virtual void SetMbcType(MbcTypes mbcType) = 0;
    virtual void SetRomBanks(int count) = 0;
    virtual void SetRamBanks(int count) = 0;
    virtual void SetMappedRomBank(int bank) = 0;
    virtual void SetMappedRamBank(int bank) = 0;
    virtual void SetBatteryBackedRam(bool hasBattery) = 0;
};