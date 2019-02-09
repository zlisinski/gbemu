#pragma once

#include <thread>
#include <vector>
#include "Buttons.h"

class Cpu;
class Memory;
class State;

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
    State *state;
};