#pragma once

#include "gbemu.h"
#include "Interrupt.h"
#include "MemoryByteObserver.h"
#include "MemoryByteSubject.h"
#include "TimerObserver.h"
#include "TimerSubject.h"


class Serial : public MemoryByteObserver, public TimerObserver
{
public:
    Serial(uint8_t *regSB, uint8_t *regSC, Interrupt* interrupts);
    virtual ~Serial();

    bool SaveState(FILE *file);
    bool LoadState(uint16_t version, FILE *file);

    virtual void AttachToMemorySubject(MemoryByteSubject* subject);
    virtual void UpdateMemoryAddr(uint16_t addr, uint8_t value);

    virtual void AttachToTimerSubject(TimerSubject* subject);
    virtual void UpdateTimer(uint value);

private:
    uint8_t *regSB;
    uint8_t *regSC;
    Interrupt* interrupts;

    uint counter;
    bool inProgress;
};