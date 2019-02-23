#pragma once

#include <thread>
#include <vector>
#include "Buttons.h"

class AbsFrameHandler;
class Cpu;
class DebugInterface;
class Display;
class Input;
class Interrupt;
class Memory;
class Serial;
class Timer;

class EmulatorMgr
{
public:
    EmulatorMgr(AbsFrameHandler *frameHandler, DebugInterface *debugInterface);
    ~EmulatorMgr();

    bool LoadRom(const char *filename);
    void ResetEmulation();
    void PauseEmulation(bool pause);
    void EndEmulation();
    void ButtonPressed(Buttons::Button button);
    void ButtonReleased(Buttons::Button button);

private:
    void ThreadFunc(std::vector<uint8_t> *gameRomMemory);

    void SetBootState(Memory *memory, Cpu *cpu);

    bool paused;
    bool quit;

    std::string romFilename;
    std::string ramFilename;

    std::thread workThread;

    AbsFrameHandler *frameHandler;
    DebugInterface *debugInterface;

    Buttons buttons;
    Cpu *cpu;
    Display *display;
    Input *input;
    Interrupt *interrupts;
    Memory *memory;
    Serial *serial;
    Timer *timer;
};