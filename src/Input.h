#pragma once

#include "gbemu.h"
#include "Buttons.h"
#include "Interrupt.h"
#include "IoRegisterProxy.h"

class Input : public IoRegisterProxy
{
public:
    Input(IoRegisterSubject *ioRegisterSubject, Interrupt* interrupts);
    virtual ~Input();

    void SetButtons(const Buttons &buttons);

    bool SaveState(FILE *file);
    bool LoadState(uint16_t version, FILE *file);

    // Inherited from IoRegisterProxy.
    virtual bool WriteByte(uint16_t address, uint8_t byte);
    virtual uint8_t ReadByte(uint16_t address) const;

private:
    void UpdateRegP1(uint8_t newP1);

    uint8_t *regP1;
    Interrupt* interrupts;

    Buttons buttonData;
};