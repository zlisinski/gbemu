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
    virtual bool GetStep() = 0;
    virtual void SetCurrentOp(uint16_t pc) = 0;
};