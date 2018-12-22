#pragma once

#include "gbemu.h"
#include "Interrupt.h"
#include "MemoryByteObserver.h"
#include "MemoryByteSubject.h"
#include "TimerObserver.h"
#include "TimerSubject.h"


class Serial : public MemoryByteObserver, public TimerObserver, public std::enable_shared_from_this<Serial>
{
public:
    Serial(uint8_t *regSB, uint8_t *regSC, std::shared_ptr<Interrupt> interrupts);
    virtual ~Serial();

    virtual void AttachToMemorySubject(std::shared_ptr<MemoryByteSubject> subject);
    virtual void UpdateMemoryAddr(uint16_t addr, uint8_t value);

    virtual void AttachToTimerSubject(TimerSubject* subject);
    virtual void UpdateTimer(uint value);

private:
    uint8_t *regSB;
    uint8_t *regSC;
    std::shared_ptr<Interrupt> interrupts;

    uint counter;
    bool inProgress;
};