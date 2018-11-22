#include "Interrupt.h"
#include "Memory.h"

const uint16_t interruptBaseAddr = 0x0040;
const uint8_t interruptOffset = 0x08;

Interrupt::Interrupt(uint8_t *regIE, uint8_t *regIF) :
    regIE(regIE),
    regIF(regIF),
    flagIME(false)
{

}


Interrupt::~Interrupt()
{
    if (memorySubject)
    {
        memorySubject->DetachObserver(eRegIE, this);
        memorySubject->DetachObserver(eRegIF, this);
    }
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


void Interrupt::AttachToMemorySubject(std::shared_ptr<MemoryByteSubject> subject)
{
    this->memorySubject = subject;

    subject->AttachObserver(eRegIE, shared_from_this());
    subject->AttachObserver(eRegIF, shared_from_this());
}


void Interrupt::UpdateMemoryAddr(uint16_t addr, uint8_t value)
{
    DBG("UpdateMemoryAddr %04X, %02X\n", addr, value);
    switch (addr)
    {
        case eRegIE:
            *regIE = value;
            break;
        case eRegIF:
            *regIF = value;
            break;
    }
}
