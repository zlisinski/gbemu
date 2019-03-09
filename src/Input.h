#pragma once

#include "gbemu.h"
#include "Buttons.h"
#include "Interrupt.h"
#include "MemoryByteObserver.h"
#include "MemoryByteSubject.h"

class Input : public MemoryByteObserver
{
public:
    Input(uint8_t *regP1, Interrupt* interrupts);
    virtual ~Input();

    void SetButtons(const Buttons &buttons);

    bool SaveState(FILE *file);
    bool LoadState(FILE *file);

    // Inherited from MemoryByteObserver.
    virtual void AttachToMemorySubject(MemoryByteSubject* subject);
    virtual void UpdateMemoryAddr(uint16_t addr, uint8_t value);

private:
    void UpdateRegP1(uint8_t newP1);

    uint8_t *regP1;
    Interrupt* interrupts;

    Buttons buttonData;
};