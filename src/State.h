#pragma once

#include "gbemu.h"

/*struct Flags
{
    uint8_t z:1; // Zero
    uint8_t n:1; // Subtract
    uint8_t h:1; // Half Carry
    uint8_t c:1; // Carry
    uint8_t pad:4;

    Flags() : z(0), n(0), h(0), c(0) {}

    void Clear()
    {
        z = n = h = c = 0;
    }
};*/

struct State
{
    union {
        struct {
            union {
                uint8_t f;
                struct {
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
    // Flags flags;
    // uint8_t a;
    union {
        struct {
            uint8_t c;
            uint8_t b;
        };
        uint16_t bc;
    };
    union {
        struct {
            uint8_t e;
            uint8_t d;
        };
        uint16_t de;
    };
    union {
        struct {
            uint8_t l;
            uint8_t h;
        };
        uint16_t hl;
    };
    uint16_t pc;
    uint16_t sp;
    uint8_t *memory;
    bool halted;

    State() : f(0), a(0), bc(0), de(0), hl(0), pc(0), sp(0), memory(NULL), halted(false)
    {
        memory = new uint8_t[MEM_SIZE]();
    }

    ~State()
    {
        delete [] memory;
    }

    void ClearFlags()
    {
        flags.z = flags.n = flags.h = flags.c = 0;
    }

    void PrintState()
    {
        printf("State: a=%02X, b=%02X, c=%02X, d=%02X, e=%02X, h=%02X, l=%02X, pc=%04X, sp=%04X, flags=z:%X n:%X h:%X c:%X\n",
               a, b, c, d, e, h, l, pc, sp, flags.z, flags.n, flags.h, flags.c);
    }
};