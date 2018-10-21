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


Input::Input(uint8_t *regP1, std::shared_ptr<Interrupt> interrupts) :
    regP1(regP1),
    interrupts(interrupts),
    buttonUp(false),
    buttonDown(false),
    buttonLeft(false),
    buttonRight(false),
    buttonSelect(false),
    buttonStart(false),
    buttonB(false),
    buttonA(false)
{
    // Start out with all buttons released.
    *regP1 = 0xFF;
}


Input::~Input()
{
    if (subject)
    {
        subject->DetachObserver(eRegP1, this);
    }
}


void Input::AttachToSubject(std::shared_ptr<MemoryByteSubject> subject)
{
    this->subject = subject;

    subject->AttachObserver(eRegP1, shared_from_this());
}


void Input::UpdateMemoryAddr(uint16_t addr, uint8_t value)
{
    DBG("UpdateMemoryAddr %04X, %02X\n", addr, value);
    if (addr == eRegP1)
        UpdateRegP1(value);
}


void Input::SetButtonUp(bool pressed)
{
    buttonUp = pressed;
    UpdateRegP1(*regP1);
}


void Input::SetButtonDown(bool pressed)
{
    buttonDown = pressed;
    UpdateRegP1(*regP1);
}


void Input::SetButtonLeft(bool pressed)
{
    buttonLeft = pressed;
    UpdateRegP1(*regP1);
}


void Input::SetButtonRight(bool pressed)
{
    buttonRight = pressed;
    UpdateRegP1(*regP1);
}


void Input::SetButtonSelect(bool pressed)
{
    buttonSelect = pressed;
    UpdateRegP1(*regP1);
}


void Input::SetButtonStart(bool pressed)
{
    buttonStart = pressed;
    UpdateRegP1(*regP1);
}


void Input::SetButtonB(bool pressed)
{
    buttonB = pressed;
    UpdateRegP1(*regP1);
}


void Input::SetButtonA(bool pressed)
{
    buttonA = pressed;
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
        if (buttonUp)
        {
            newP1 &= eMaskSelectUp;
            DBG("Up pressed\n");
        }
        if (buttonDown)
        {
            newP1 &= eMaskStartDown;
            DBG("Down pressed\n");
        }
        if (buttonLeft)
        {
            newP1 &= eMaskBLeft;
            DBG("Left pressed\n");
        }
        if (buttonRight)
        {
            newP1 &= eMaskARight;
            DBG("Right pressed\n");
        }
    }
    else if ((newP1 & eMaskButtons) == 0)
    {
        if (buttonSelect)
        {
            newP1 &= eMaskSelectUp;
            DBG("Select pressed\n");
        }
        if (buttonStart)
        {
            newP1 &= eMaskStartDown;
            DBG("Start pressed\n");
        }
        if (buttonB)
        {
            newP1 &= eMaskBLeft;
            DBG("B pressed\n");
        }
        if (buttonA)
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