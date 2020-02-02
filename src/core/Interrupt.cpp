#include <iomanip>
#include <memory>
#include <sstream>

#include "Interrupt.h"
#include "Logger.h"
#include "Memory.h"

const uint16_t interruptBaseAddr = 0x0040;
const uint8_t interruptOffset = 0x08;

Interrupt::Interrupt(IoRegisterSubject *ioRegisterSubject) :
    regIE(ioRegisterSubject->AttachIoRegister(eRegIE, this)),
    regIF(ioRegisterSubject->AttachIoRegister(eRegIF, this)),
    flagIME(false)
{

}


Interrupt::~Interrupt()
{

}


bool Interrupt::CheckInterrupts(eInterruptTypes &intType)
{
    uint8_t interrupts = *regIE & *regIF;

    if (interrupts & eIntBitVBlank)
    {
        intType = eIntVBlank;
        return true;
    }

    if (interrupts & eIntBitLCDC)
    {
        intType = eIntLCDC;
        return true;
    }

    if (interrupts & eIntBitTimer)
    {
        intType = eIntTimer;
        return true;
    }

    if (interrupts & eIntBitSerial)
    {
        intType = eIntSerial;
        return true;
    }

    if (interrupts & eIntBitJoypad)
    {
        intType = eIntJoypad;
        return true;
    }

    return false;
}


uint16_t Interrupt::GetInterruptAddress(eInterruptTypes type)
{
    return interruptBaseAddr + (interruptOffset * type);
}


void Interrupt::RequestInterrupt(eInterruptTypes type)
{
    *regIF |= (1 << type);
}


void Interrupt::ClearInterrupt(eInterruptTypes type)
{
    *regIF &= ~(1 << type);
}


bool Interrupt::SaveState(FILE *file)
{
    if (!fwrite(&flagIME, sizeof(flagIME), 1, file))
        return false;

    return true;
}


bool Interrupt::LoadState(uint16_t version, FILE *file)
{
    (void)version;

    if (!fread(&flagIME, sizeof(flagIME), 1, file))
        return false;

    return true;
}


bool Interrupt::WriteByte(uint16_t address, uint8_t byte)
{
    LogInstruction("Interrupt::WriteByte %04X, %02X", address, byte);

    switch (address)
    {
        case eRegIE:
            // Mooneye's tests say unused bits are whatever they were set to.
            *regIE = byte;
            return true;
        case eRegIF:
            // Mooneye's tests say unused bits are set to 1.
            *regIF = byte | 0xE0;
            return true;
        default:
            return false;
    }
}


uint8_t Interrupt::ReadByte(uint16_t address) const
{
    LogInstruction("Interrupt::ReadByte %04X", address);

    switch (address)
    {
        case eRegIE:
            // Mooneye's tests say unused bits are whatever they were set to.
            return *regIE;
        case eRegIF:
            // Mooneye's tests say unused bits are set to 1.
            return *regIF | 0xE0;
        default:
            std::stringstream ss;
            ss << "Interrupt doesnt handle reads to 0x" << std::hex << std::setw(4) << std::setfill('0') << address;
            throw std::range_error(ss.str());
    }
}