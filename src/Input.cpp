#include <memory>

#include "Input.h"
#include "Memory.h"

enum ButtonMask
{
    eMaskARight    = ~0x01, // 0xFE
    eMaskBLeft     = ~0x02, // 0xFD
    eMaskSelectUp  = ~0x04, // 0xFB
    eMaskStartDown = ~0x08, // 0xF7
    eMaskDirection = 0x10, // 0xEF
    eMaskButtons   = 0x20  // 0xDF
};


Input::Input(uint8_t *regP1, Interrupt* interrupts) :
    regP1(regP1),
    interrupts(interrupts),
    buttonData()
{
    // Start out with all buttons released.
    *regP1 = 0xFF;
}


Input::~Input()
{
    if (memorySubject)
    {
        memorySubject->DetachObserver(eRegP1, this);
    }
}


void Input::AttachToMemorySubject(MemoryByteSubject* subject)
{
    this->memorySubject = subject;

    subject->AttachObserver(eRegP1, this);
}


void Input::UpdateMemoryAddr(uint16_t addr, uint8_t value)
{
    DBG("UpdateMemoryAddr %04X, %02X\n", addr, value);
    if (addr == eRegP1)
        UpdateRegP1(value);
}


void Input::SetButtons(const Buttons &buttons)
{
    buttonData = buttons;
    UpdateRegP1(*regP1);
}


void Input::UpdateRegP1(uint8_t newP1)
{
    if ((newP1 & 0x30) == 0x30)
    {
        // No lines are selected, so no button bits can be set.
        *regP1 = 0xFF;
        return;
    }

    // Clear all buttons before checking them below. Top two bits are always set.
    newP1 |= 0xCF;

    if ((newP1 & eMaskDirection) == 0)
    {
        if (buttonData.IsUpPressed())
        {
            newP1 &= eMaskSelectUp;
            DBG("Up pressed\n");
        }
        if (buttonData.IsDownPressed())
        {
            newP1 &= eMaskStartDown;
            DBG("Down pressed\n");
        }
        if (buttonData.IsLeftPressed())
        {
            newP1 &= eMaskBLeft;
            DBG("Left pressed\n");
        }
        if (buttonData.IsRightPressed())
        {
            newP1 &= eMaskARight;
            DBG("Right pressed\n");
        }
    }
    else if ((newP1 & eMaskButtons) == 0)
    {
        if (buttonData.IsSelectPressed())
        {
            newP1 &= eMaskSelectUp;
            DBG("Select pressed\n");
        }
        if (buttonData.IsStartPressed())
        {
            newP1 &= eMaskStartDown;
            DBG("Start pressed\n");
        }
        if (buttonData.IsBPressed())
        {
            newP1 &= eMaskBLeft;
            DBG("B pressed\n");
        }
        if (buttonData.IsAPressed())
        {
            newP1 &= eMaskARight;
            DBG("A pressed\n");
        }
    }

    bool triggerInterrupt = false;

    // If no buttons were previously pushed, and now some are, trigger an interrupt.
    if (((*regP1 & 0x0F) == 0x0F) && ((newP1 & 0x0F) != 0x0F))
        triggerInterrupt = true;

    *regP1 = newP1;

    if (triggerInterrupt && interrupts)
        interrupts->RequestInterrupt(eIntJoypad);
}