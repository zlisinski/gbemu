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

    State(void (*drawFrameCallback)(uint32_t *));
    ~State();

    void SetRomMemory(std::array<uint8_t, BOOT_ROM_SIZE> &bootRomMemory, std::vector<uint8_t> &gameRomMemory);
    void SetRomMemory(std::vector<uint8_t> &gameRomMemory);

private:
    void SetBootState();
};