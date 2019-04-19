#include <iomanip>
#include <memory>
#include <sstream>

#include "Input.h"
#include "Logger.h"
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


Input::Input(IoRegisterSubject *ioRegisterSubject, Interrupt *interrupts) :
    regP1(ioRegisterSubject->AttachIoRegister(eRegP1, this)),
    interrupts(interrupts),
    buttonData()
{
    // Start out with all buttons released.
    *regP1 = 0xFF;
}


Input::~Input()
{

}


bool Input::WriteByte(uint16_t address, uint8_t byte)
{
   LogInstruction("Input::WriteByte %04X, %02X", address, byte);

    switch (address)
    {
        case eRegP1:
            UpdateRegP1(byte);
            return true;
        default:
            return false;
    }
}


uint8_t Input::ReadByte(uint16_t address) const
{
    LogInstruction("Input::ReadByte %04X", address);

    switch (address)
    {
        case eRegP1:
            return *regP1;
        default:
            std::stringstream ss;
            ss << "Input doesnt handle reads to 0x" << std::hex << std::setw(4) << std::setfill('0') << address;
            throw std::range_error(ss.str());
    }
}


void Input::SetButtons(const Buttons &buttons)
{
    buttonData = buttons;
    UpdateRegP1(*regP1);
}


bool Input::SaveState(FILE *file)
{
    if (!fwrite(&buttonData.data, sizeof(buttonData.data), 1, file))
        return false;

    return true;
}


bool Input::LoadState(uint16_t version, FILE *file)
{
    (void)version;

    if (!fread(&buttonData.data, sizeof(buttonData.data), 1, file))
        return false;

    return true;
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
            //LogDebug("Up pressed");
        }
        if (buttonData.IsDownPressed())
        {
            newP1 &= eMaskStartDown;
            //LogDebug("Down pressed");
        }
        if (buttonData.IsLeftPressed())
        {
            newP1 &= eMaskBLeft;
            //LogDebug("Left pressed");
        }
        if (buttonData.IsRightPressed())
        {
            newP1 &= eMaskARight;
            //LogDebug("Right pressed");
        }
    }
    else if ((newP1 & eMaskButtons) == 0)
    {
        if (buttonData.IsSelectPressed())
        {
            newP1 &= eMaskSelectUp;
            //LogDebug("Select pressed");
        }
        if (buttonData.IsStartPressed())
        {
            newP1 &= eMaskStartDown;
            //LogDebug("Start pressed");
        }
        if (buttonData.IsBPressed())
        {
            newP1 &= eMaskBLeft;
            //LogDebug("B pressed");
        }
        if (buttonData.IsAPressed())
        {
            newP1 &= eMaskARight;
            //LogDebug("A pressed");
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