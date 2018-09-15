#pragma once

#include <memory>

#include "gbemu.h"
#include "State.h"
#include "AbsByteProxy.h"
#include "Interrupt.h"


typedef std::shared_ptr<AbsByteProxy> ByteProxy;

class Emulator
{
public:
    Emulator(State *state);

    uint8_t Read8bit();
    uint16_t Read16bit();

    uint8_t HighByte(uint16_t word);
    uint8_t LowByte(uint16_t word);
    uint8_t GetFlagValue(uint8_t bits);

    uint8_t Add8Bit(uint8_t x, uint8_t y, bool carryFlag = false);
    uint16_t Add16Bit(uint16_t x, uint16_t y);
    uint16_t Add16BitSigned8Bit(uint16_t x, int8_t y);
    uint8_t Sub8Bit(uint8_t x, uint8_t y, bool carryFlag = false);

    void Push(uint16_t src);
    void Pop(uint16_t *dest);

    uint ProcessOpCode();

private:
    ByteProxy GetByteProxy(uint8_t bits);
    void NotYetImplemented();

    void ProcessInterrupt(eInterruptTypes intType);

    State *state;
    bool enableInterruptsDelay;
    bool haltBug;

    uint8_t *regMap8Bit[8];
    uint16_t *regMap16Bit[4];
    uint16_t *regMap16BitStack[4];
    static const char *regNameMap8Bit[8];
    static const char *regNameMap16Bit[4];
    static const char *regNameMap16BitStack[4];
    static const char *flagNameMap[4];
};