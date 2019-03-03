#include <memory>
#include "Serial.h"
#include "Logger.h"
#include "Memory.h"

const uint cyclesPerBit = 8;

Serial::Serial(uint8_t *regSB, uint8_t *regSC, Interrupt* interrupts) :
    regSB(regSB),
    regSC(regSC),
    interrupts(interrupts),
    counter(0),
    inProgress(false)
{
    *regSB = 0xFF;
    *regSC = 0x7F;
}


Serial::~Serial()
{
    if (memorySubject)
        memorySubject->DetachObserver(eRegSC, this);

    if (timerSubject)
        timerSubject->DetachObserver(this);
}


void Serial::AttachToMemorySubject(MemoryByteSubject* subject)
{
   this->memorySubject = subject;
    subject->AttachObserver(eRegSC, this);
}


void Serial::UpdateMemoryAddr(uint16_t addr, uint8_t value)
{
    if (addr != eRegSC)
        return;

    // Unused regSC bits are always 1.
    *regSC = value | 0x7E;

    // Only start transfer if using internal clock source.
    if (!inProgress && (value & 0x81) == 0x81) 
    {
        inProgress = true;
        counter = 0;
    }
}


void Serial::AttachToTimerSubject(TimerSubject* subject)
{
    this->timerSubject = subject;
    subject->AttachObserver(this);
}


void Serial::UpdateTimer(uint value)
{
    if (!inProgress)
        return;

    counter += value;

    // For now, process the transfer in one chunk, instead of a bit at a time. Change later if needed.
    if (counter >= cyclesPerBit * 8 * 4)
    {
        // Write byte to debug file.
        FILE *file = fopen("serial.txt", "a");
        fputc(*regSB, file);
        fclose(file);
        LogDebug("Serial: %02X, '%c'", *regSB, *regSB);

        inProgress = false;

        // Bits are shifted in when bits are shifted out. Since we aren't emulating a connected serial
        // device, 0xFF gets shifted in.
        *regSB = 0xFF;

        // Clear transfer start flag.
        *regSC &= 0x7F;

        if (interrupts)
            interrupts->RequestInterrupt(eIntSerial);
    }
}