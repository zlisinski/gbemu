#pragma once

#include "gbemu.h"
#include "Display.h"
#include "Input.h"
#include "Interrupt.h"
#include "Memory.h"
#include "Timer.h"

class State
{
public:
    // Yes, I know that this is probably Undefined Behavior in Standard C++. Type punning works in C (at least in GCC), but I'm having
    // trouble nailing down whether it is accepted in G++. And I know about the the "warning: ISO C++ prohibits anonymous structs"
    // warnings. I'm ignoring them for now, as this lets me focus on getting the rest of the emulator working. Regardless, it works
    // for now, so I'm not going to change it until I care about cross platform/compiler, or even anything other than G++ 7.3.0 (LLVM,
    // Visual Studio, etc). This is a personal project, and is not meant to compile/run in whatever _your_ chosen platform or compiler is.
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
    std::shared_ptr<Memory> memory;
    std::shared_ptr<Interrupt> interrupts;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<Display> display;
    std::shared_ptr<Input> input;
    bool halted;

    State();
    ~State();

    void SetRomMemory(std::array<uint8_t, BOOT_ROM_SIZE> &bootRomMemory, std::vector<uint8_t> &gameRomMemory);
    void SetRomMemory(std::vector<uint8_t> &gameRomMemory);

    inline void ClearFlags()
    {
        flags.z = flags.n = flags.h = flags.c = 0;
    }

    inline void PrintState()
    {
        DBG("State: a=%02X, b=%02X, c=%02X, d=%02X, e=%02X, h=%02X, l=%02X, pc=%04X, sp=%04X, flags=z:%X n:%X h:%X c:%X int:%d\n",
               a, b, c, d, e, h, l, pc, sp, flags.z, flags.n, flags.h, flags.c, interrupts->Enabled());
    }

private:
    void SetBootState();
};