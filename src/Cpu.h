#pragma once

#include <memory>

#include "gbemu.h"
#include "State.h"
#include "AbsByteProxy.h"
#include "Interrupt.h"

// Yes, I know that this is probably Undefined Behavior in Standard C++. Type punning works in C (at least in GCC), but I'm having
// trouble nailing down whether it is accepted in G++. And I know about the the "warning: ISO C++ prohibits anonymous structs"
// warnings. I'm ignoring them for now, as this lets me focus on getting the rest of the emulator working. Regardless, it works
// for now, so I'm not going to change it until I care about cross platform/compiler, or even anything other than G++ 7.3.0 (clang,
// Visual Studio, etc). This is a personal project, and is not meant to compile/run in whatever _your_ chosen platform or compiler is.
struct Registers
{
    Registers() :
        af(0),
        bc(0),
        de(0),
        hl(0),
        pc(0),
        sp(0)
    {}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    union
    {
        struct
        {
            union
            {
                uint8_t f;
                struct
                {
                    uint8_t pad:4;
                    uint8_t c:1; // Carry
                    uint8_t h:1; // Half Carry
                    uint8_t n:1; // Subtract
                    uint8_t z:1; // Zero
                } flags;
            };
            uint8_t a;
        };
        uint16_t af;
    };
    union
    {
        struct
        {
            uint8_t c;
            uint8_t b;
        };
        uint16_t bc;
    };
    union
    {
        struct
        {
            uint8_t e;
            uint8_t d;
        };
        uint16_t de;
    };
    union
    {
        struct
        {
            uint8_t l;
            uint8_t h;
        };
        uint16_t hl;
    };
#pragma GCC diagnostic pop

    uint16_t pc;
    uint16_t sp;
};


typedef std::unique_ptr<AbsByteProxy> ByteProxy;


class Cpu
{
public:
    Cpu(State *state);

    uint8_t ReadPC8Bit();
    uint16_t ReadPC16Bit();

    uint8_t HighByte(uint16_t word);
    uint8_t LowByte(uint16_t word);
    uint8_t GetFlagValue(uint8_t bits);

    uint8_t Add8Bit(uint8_t x, uint8_t y, bool carryFlag = false);
    uint16_t Add16Bit(uint16_t x, uint16_t y);
    uint16_t Add16BitSigned8Bit(uint16_t x, int8_t y);
    uint8_t Sub8Bit(uint8_t x, uint8_t y, bool carryFlag = false);

    void Push(uint16_t src);
    void Pop(uint16_t *dest);

    void ProcessOpCode();

    inline void ClearFlags()
    {
        reg.flags.z = reg.flags.n = reg.flags.h = reg.flags.c = 0;
    }

    Registers reg;

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