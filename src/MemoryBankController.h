#pragma once

#include <memory>

#include "gbemu.h"

class AbsMbc;

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
public:
    virtual void MapRomBank(uint8_t bank) = 0;
    virtual void MapRamBank(uint8_t bank) = 0;
    virtual void EnableRam(bool enable) = 0;
};


// MbcFactory /////////////////////////////////////////////////////////////////////////////////////


class MbcFactory
{
public:
    static std::unique_ptr<AbsMbc> GetMbcInstance(MbcTypes mbcType, MemoryBankInterface *memory);
};


// AbsMbc /////////////////////////////////////////////////////////////////////////////////////////


class AbsMbc
{
public:
    AbsMbc(MemoryBankInterface *memory);
    virtual ~AbsMbc() {}

    virtual void WriteByte(uint16_t addr, uint8_t byte) = 0;

    virtual bool SaveState(FILE *file) = 0;
    virtual bool LoadState(uint16_t version, FILE *file) = 0;

protected:
    MemoryBankInterface *memory;
};


// MbcNone ////////////////////////////////////////////////////////////////////////////////////////


class MbcNone : public AbsMbc
{
public:
    MbcNone(MemoryBankInterface *memory);

    virtual void WriteByte(uint16_t addr, uint8_t byte);

    virtual bool SaveState(FILE *file);
    virtual bool LoadState(uint16_t version, FILE *file);
};


// Mbc1 ///////////////////////////////////////////////////////////////////////////////////////////


class Mbc1 : public AbsMbc
{
public:
    Mbc1(MemoryBankInterface *memory);

    virtual void WriteByte(uint16_t addr, uint8_t byte);

    virtual bool SaveState(FILE *file);
    virtual bool LoadState(uint16_t version, FILE *file);

private:
    uint8_t regRamEnable;
    uint8_t regRomLowBits;
    uint8_t regRomHighBits;
    uint8_t regRamMode;
};


// Mbc2 ///////////////////////////////////////////////////////////////////////////////////////////


class Mbc2 : public AbsMbc
{
public:
    Mbc2(MemoryBankInterface *memory);

    virtual void WriteByte(uint16_t addr, uint8_t byte);

    virtual bool SaveState(FILE *file);
    virtual bool LoadState(uint16_t version, FILE *file);

private:
    uint8_t regRamEnable;
    uint8_t regRomBank;
};


// Mbc3 ///////////////////////////////////////////////////////////////////////////////////////////


class Mbc3 : public AbsMbc
{
public:
    Mbc3(MemoryBankInterface *memory);

    virtual void WriteByte(uint16_t addr, uint8_t byte);

    virtual bool SaveState(FILE *file);
    virtual bool LoadState(uint16_t version, FILE *file);

private:
    uint8_t regRamEnable;
    uint8_t regRomBank;
    uint8_t regRamBank;
    uint8_t regRtcLatch;
};