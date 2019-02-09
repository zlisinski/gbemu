#pragma once

#include "gbemu.h"
#include "Display.h"
#include "Input.h"
#include "Interrupt.h"
#include "Memory.h"
#include "Serial.h"
#include "Timer.h"

class State
{
public:

    Memory* memory;
    Interrupt* interrupts;
    Timer* timer;
    Display* display;
    Input* input;
    Serial* serial;
    bool halted;

    State(void (*drawFrameCallback)(uint32_t *));
    ~State();

    void SetRomMemory(std::array<uint8_t, BOOT_ROM_SIZE> &bootRomMemory, std::vector<uint8_t> &gameRomMemory);
    void SetRomMemory(std::vector<uint8_t> &gameRomMemory);

    /*inline void PrintState()
    {
        DBG("State: a=%02X, b=%02X, c=%02X, d=%02X, e=%02X, h=%02X, l=%02X, pc=%04X, sp=%04X, flags=z:%X n:%X h:%X c:%X int:%d\n",
               a, b, c, d, e, h, l, pc, sp, flags.z, flags.n, flags.h, flags.c, interrupts->Enabled());
    }*/

private:
    void SetBootState();
};