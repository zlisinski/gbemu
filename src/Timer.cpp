#include <memory>

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


Timer::Timer(uint8_t *regTIMA, uint8_t *regTMA, uint8_t *regTAC, uint8_t *regDIV, Interrupt* interrupts) :
    regTIMA(regTIMA),
    regTMA(regTMA),
    regTAC(regTAC),
    regDIV(regDIV),
    internalCounter(0),
    regTIMAOverflowed(false),
    interrupts(interrupts)
{
    
}


Timer::~Timer()
{
    if (memorySubject)
    {
        //memorySubject->DetachObserver(eRegTIMA, this);
        //memorySubject->DetachObserver(eRegTMA, this);
        memorySubject->DetachObserver(eRegTAC, this);
        memorySubject->DetachObserver(eRegDIV, this);
    }
}


// This has to be called after construction.
void Timer::AttachToMemorySubject(MemoryByteSubject* subject)
{
    this->memorySubject = subject;
    //subject->AttachObserver(eRegTIMA, this);
    //subject->AttachObserver(eRegTMA, this);
    subject->AttachObserver(eRegTAC, this);
    subject->AttachObserver(eRegDIV, this);
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
            DBG("Timer overflow\n");
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


void Timer::UpdateMemoryAddr(uint16_t addr, uint8_t value)
{
    DBG("UpdateMemoryAddr %04X, %02X\n", addr, value);
    switch (addr)
    {
        /*case eRegTIMA:
            break;
        case eRegTMA:
            break;*/
        case eRegTAC:
            WriteTAC(value);
            break;
        case eRegDIV:
            WriteDIV();
            break;
    }
}


void Timer::PrintTimerData()
{
    DBG("Timer: TIMA=%02X, TMA=%02X, TAC=%02X, DIV=%02x, counter=%02X\n", *regTIMA, *regTMA, *regTAC, *regDIV, internalCounter);
}