#include <iomanip>
#include <memory>
#include <sstream>

#include "Serial.h"
#include "Logger.h"
#include "Memory.h"

const uint cyclesPerBit = 8;

Serial::Serial(IoRegisterSubject *ioRegisterSubject, Interrupt* interrupts) :
    regSB(ioRegisterSubject->AttachIoRegister(eRegSB, this)),
    regSC(ioRegisterSubject->AttachIoRegister(eRegSC, this)),
    interrupts(interrupts),
    counter(0),
    inProgress(false)
{
    *regSB = 0xFF;
    *regSC = 0x7F;
}


Serial::~Serial()
{
    if (timerSubject)
        timerSubject->DetachObserver(this);
}


bool Serial::SaveState(FILE *file)
{
    if (!fwrite(&counter, sizeof(counter), 1, file))
        return false;

    if (!fwrite(&inProgress, sizeof(inProgress), 1, file))
        return false;

    return true;
}


bool Serial::LoadState(uint16_t version, FILE *file)
{
    (void)version;

    if (!fread(&counter, sizeof(counter), 1, file))
        return false;

    if (!fread(&inProgress, sizeof(inProgress), 1, file))
        return false;

    return true;
}


bool Serial::WriteByte(uint16_t address, uint8_t byte)
{
   LogInstruction("Serial::WriteByte %04X, %02X", address, byte);

    switch (address)
    {
        case eRegSB:
            *regSB = byte;
            return true;
        case eRegSC:
            // Unused regSC bits are always 1.
            *regSC = byte | 0x7E;

            // Only start transfer if using internal clock source.
            if (!inProgress && (byte & 0x81) == 0x81)
            {
                inProgress = true;
                counter = 0;
            }
            return true;
        default:
            return false;
    }
}


uint8_t Serial::ReadByte(uint16_t address) const
{
    LogInstruction("Serial::ReadByte %04X", address);

    switch (address)
    {
        case eRegSB:
            return *regSB;
        case eRegSC:
            return *regSC;
        default:
            std::stringstream ss;
            ss << "Serial doesnt handle reads to 0x" << std::hex << std::setw(4) << std::setfill('0') << address;
            throw std::range_error(ss.str());
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