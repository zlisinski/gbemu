#pragma once

#include "gbemu.h"

class Cpu;
class Interrupt;
class Memory;

class DebuggerInterface
{
public:
    virtual void SetEmulatorObjects(Memory *newMemory, Cpu *newCpu, Interrupt *newInterrupt) = 0;

    virtual bool GetDebuggingEnabled() = 0;
    virtual bool ShouldRun(uint16_t pc) = 0;
    virtual void SetCurrentOp(uint16_t pc) = 0;

    virtual void MemoryChanged(uint16_t address, uint16_t len) = 0;
};