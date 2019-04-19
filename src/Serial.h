#pragma once

#include "gbemu.h"
#include "IoRegisterProxy.h"
#include "Interrupt.h"
#include "TimerObserver.h"


class Serial : public IoRegisterProxy, public TimerObserver
{
public:
    Serial(IoRegisterSubject *ioRegisterSubject, Interrupt *interrupts, TimerSubject *timerSubject);
    virtual ~Serial();

    bool SaveState(FILE *file);
    bool LoadState(uint16_t version, FILE *file);

    // Inherited from IoRegisterProxy.
    virtual bool WriteByte(uint16_t address, uint8_t byte);
    virtual uint8_t ReadByte(uint16_t address) const;

    // Inherited from TimerObserver.
    virtual void UpdateTimer(uint value);

private:
    uint8_t *regSB;
    uint8_t *regSC;
    Interrupt *interrupts;

    uint counter;
    bool inProgress;
};