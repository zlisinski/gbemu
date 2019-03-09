#pragma once

#include "gbemu.h"

enum MbcTypes
{
    eMbcNone,
    eMbc1,
    eMbc2,
    eMbc3,
    eMbc5
};

class MemoryBankInterface
{
protected:
    virtual void MapRomBank(uint bank) = 0;
    virtual void MapRamBank(uint bank) = 0;
    virtual void EnableRam(bool enable) = 0;

    friend class MemoryBankController;
};

class MemoryBankController
{
public:
    MemoryBankController(MbcTypes mbcType, uint8_t romBankCount, uint8_t ramBankCount, MemoryBankInterface *memory);
    ~MemoryBankController();

    void WriteByte(uint16_t addr, uint8_t byte);

    bool SaveState(FILE *file);
    bool LoadState(FILE *file);

private:
    MbcTypes mbcType;
    uint8_t romBankCount;
    uint8_t ramBankCount;
    MemoryBankInterface *memory;

    uint8_t register0;
    uint8_t register1;
    uint8_t register2;
    uint8_t register3;
};