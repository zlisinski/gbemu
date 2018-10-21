#pragma once

#include "gbemu.h"
#include "Interrupt.h"
#include "MemoryByteObserver.h"
#include "MemoryByteSubject.h"

class Input : public MemoryByteObserver, public std::enable_shared_from_this<Input>
{
public:
    Input(uint8_t *regP1, std::shared_ptr<Interrupt> interrupts);
    virtual ~Input();

    void SetButtonUp(bool pressed);
    void SetButtonDown(bool pressed);
    void SetButtonLeft(bool pressed);
    void SetButtonRight(bool pressed);
    void SetButtonSelect(bool pressed);
    void SetButtonStart(bool pressed);
    void SetButtonB(bool pressed);
    void SetButtonA(bool pressed);

    // Inherited from MemoryByteObserver.
    virtual void AttachToSubject(std::shared_ptr<MemoryByteSubject> subject);
    virtual void UpdateMemoryAddr(uint16_t addr, uint8_t value);

private:
    void UpdateRegP1(uint8_t newP1);

    uint8_t *regP1;
    std::shared_ptr<Interrupt> interrupts;

    bool buttonUp;
    bool buttonDown;
    bool buttonLeft;
    bool buttonRight;
    bool buttonSelect;
    bool buttonStart;
    bool buttonB;
    bool buttonA;
};