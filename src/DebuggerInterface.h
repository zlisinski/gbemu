#pragma once

#include "gbemu.h"

class Cpu;
class Interrupt;
class Memory;

class DebuggerInterface
{
public:
    virtual void SetMemory(Memory *memory) = 0;
    virtual void SetCpu(Cpu *cpu) = 0;
    virtual void SetInterrupt(Interrupt *interrupt) = 0;

    virtual bool GetDebuggingEnabled() = 0;
    virtual bool ShouldRun() = 0;
    virtual void SetCurrentOp(uint16_t pc) = 0;

    virtual void MemoryChanged(uint16_t address, uint16_t len) = 0;
};