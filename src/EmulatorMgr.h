#pragma once

#include <mutex>
#include <thread>
#include <vector>
#include "Buttons.h"

class AbsFrameHandler;
class Audio;
class Cpu;
class DebuggerInterface;
class Display;
class InfoInterface;
class Input;
class Interrupt;
class Memory;
class Serial;
class Timer;

class EmulatorMgr
{
public:
    EmulatorMgr(AbsFrameHandler *frameHandler, InfoInterface *infoInterface, DebuggerInterface *debuggerInterface);
    ~EmulatorMgr();

    bool LoadRom(const char *filename);
    void ResetEmulation();
    void PauseEmulation(bool pause);
    void EndEmulation();
    void ButtonPressed(Buttons::Button button);
    void ButtonReleased(Buttons::Button button);

    void SaveState(int slot);
    void LoadState(int slot);

private:
    void ThreadFunc();

    void SetBootState(Memory *memory, Cpu *cpu);

    bool paused;
    bool quit;

    std::vector<uint8_t> gameRomMemory;

    std::string romFilename;
    std::string ramFilename;

    std::thread workThread;
    std::mutex saveStateMutex;

    AbsFrameHandler *frameHandler;
    InfoInterface *infoInterface;
    DebuggerInterface *debuggerInterface;

    Audio *audio;
    Buttons buttons;
    Cpu *cpu;
    Display *display;
    Input *input;
    Interrupt *interrupts;
    Memory *memory;
    Serial *serial;
    Timer *timer;
};