#pragma once

#include <thread>
#include <vector>
#include "Buttons.h"

class Cpu;
class Display;
class Input;
class Interrupt;
class Memory;
class Serial;
class Timer;

class EmulatorMgr
{
public:
    EmulatorMgr(void (*drawFrameCallback)(uint32_t *));
    ~EmulatorMgr();

    bool LoadRom(const char *filename);
    void PauseEmulation(bool pause);
    void QuitEmulation();
    void ButtonPressed(Buttons::Button button);
    void ButtonReleased(Buttons::Button button);

private:
    void ThreadFunc(std::vector<uint8_t> *gameRomMemory);

    void SetBootState(Memory *memory, Cpu *cpu);

    bool paused;
    bool quit;

    std::string romFilename;

    std::thread workThread;

    void (*drawFrameCallback)(uint32_t *);

    Buttons buttons;
    Cpu *cpu;
    Display *display;
    Input *input;
    Interrupt *interrupts;
    Memory *memory;
    Serial *serial;
    Timer *timer;
};