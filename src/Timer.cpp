#include <iomanip>
#include <memory>
#include <sstream>

#include "Logger.h"
#include "Timer.h"


// The lower two bits of the TAC register control how many clock cycles occur before TIMA is updated.
// DIV and the internal counter byte make up the 16 bit counter, with DIV being the high byte.
// It works by selecting a certain bit from DIV+internalCounter, and when that bit overflows, TIMA is incremented.
// 0 = 1024 clocks, bit 1 of DIV / bit 9 of combined counter overflows.
// 1 = 16 clocks, bit 3 of internalCounter overflows.
// 2 = 64 clocks, bit 5 of internalCounter overflows.
// 3 = 256 clocks, bit 7 of internalCounter overflows.
const uint16_t frequencyMapMask[] = {0x0200, 0x0008, 0x0020, 0x0080};
const uint8_t timerEnabledMask = 0x04;
const uint8_t clocksPerCycle = 4;


Timer::Timer(IoRegisterSubject *ioRegisterSubject, Interrupt *interrupts) :
    regTIMA(ioRegisterSubject->AttachIoRegister(eRegTIMA, this)),
    regTMA(ioRegisterSubject->AttachIoRegister(eRegTMA, this)),
    regTAC(ioRegisterSubject->AttachIoRegister(eRegTAC, this)),
    regDIV(ioRegisterSubject->AttachIoRegister(eRegDIV, this)),
    internalCounter(0),
    regTIMAOverflowed(false),
    interrupts(interrupts)
{
    
}


Timer::~Timer()
{

}


bool Timer::WriteByte(uint16_t address, uint8_t byte)
{
   LogInstruction("Timer::WriteByte %04X, %02X", address, byte);

    switch (address)
    {
        case eRegTIMA:
            *regTIMA = byte;
            return true;
        case eRegTMA:
            *regTMA = byte;
            return true;
        case eRegTAC:
            WriteTAC(byte);
            return true;
        case eRegDIV:
            WriteDIV();
            return true;
        default:
            return false;
    }
}


uint8_t Timer::ReadByte(uint16_t address) const
{
    LogInstruction("Timer::ReadByte %04X", address);

    switch (address)
    {
        case eRegTIMA:
            return *regTIMA;
        case eRegTMA:
            return *regTMA;
        case eRegTAC:
            // Unused bits are set to 1.
            return *regTAC | 0xF8;
        case eRegDIV:
            return *regDIV;
        default:
            std::stringstream ss;
            ss << "Timer doesnt handle reads to 0x" << std::hex << std::setw(4) << std::setfill('0') << address;
            throw std::range_error(ss.str());
    }
}


void Timer::AddCycle()
{
    // Copy TMA to TIMA after a delay when TIMA overflows.
    if (regTIMAOverflowed)
    {
        *regTIMA = *regTMA;

        // Set timer interrupt.
        interrupts->RequestInterrupt(eIntTimer);

        regTIMAOverflowed = false;
    }

    uint16_t currentCounter = (*regDIV << 8) | internalCounter;
    uint16_t newCounter = currentCounter + clocksPerCycle;

    ProcessCounterChange(currentCounter, newCounter);

    *regDIV = newCounter >> 8;
    internalCounter = newCounter & 0xFF;

    NotifyObservers(clocksPerCycle);
}


void Timer::ProcessCounterChange(uint16_t oldValue, uint16_t newValue)
{
    // Return if the timer is not enabled.
    if (!(*regTAC & timerEnabledMask))
        return;
    
    uint16_t frequencySelectMask = frequencyMapMask[*regTAC & 0x03];

    //DBG("CounterChange: %u %u %u\n", frequencySelectMask, (oldValue & frequencySelectMask), (newValue & frequencySelectMask));

    // Check for an overflow of the bit selected by TAC.
    if ((oldValue & frequencySelectMask) && !(newValue & frequencySelectMask))
    {
        // Incrememt TIMA,
        (*regTIMA)++;

        // If TIMA overflowed, set the copy of TMA to TIMA on the next cycle.
        if (*regTIMA == 0)
        {
            LogDebug("Timer overflow");
            regTIMAOverflowed = true;
        }
    }
}


void Timer::WriteDIV()
{
    // Any write to the DIV register clears DIV and the internal counter.

    uint16_t currentCounter = (*regDIV << 8) | internalCounter;
    ProcessCounterChange(currentCounter, 0);

    *regDIV = 0;
    internalCounter = 0;
}


void Timer::WriteTAC(uint8_t newValue)
{
    // TODO: Check for frequency change, and bits associated with old freq and new freq.
    *regTAC = newValue;
}


void Timer::PrintTimerData()
{
    LogDebug("Timer: TIMA=%02X, TMA=%02X, TAC=%02X, DIV=%02x, counter=%02X", *regTIMA, *regTMA, *regTAC, *regDIV, internalCounter);
}


bool Timer::SaveState(FILE *file)
{
    if (!fwrite(&internalCounter, sizeof(internalCounter), 1, file))
        return false;

    if (!fwrite(&regTIMAOverflowed, sizeof(regTIMAOverflowed), 1, file))
        return false;

    return true;
}


bool Timer::LoadState(uint16_t version, FILE *file)
{
    (void)version;

    if (!fread(&internalCounter, sizeof(internalCounter), 1, file))
        return false;

    if (!fread(&regTIMAOverflowed, sizeof(regTIMAOverflowed), 1, file))
        return false;

    return true;
}