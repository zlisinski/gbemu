#pragma once

#include "gbemu.h"
#include "Interrupt.h"
#include "Memory.h"
#include "MemoryByteObserver.h"
#include "MemoryByteSubject.h"
#include "TimerSubject.h"

// The memory for the timer registers belongs to the Memory class, but the Timer class 'owns' the access to the memory.
// Because of this, we use direct pointers to the memory, and any writes that happen through the Memory class will get
// sent to this class for processing.
class Timer : public MemoryByteObserver, public TimerSubject
{
public:
    Timer(uint8_t *regTIMA, uint8_t *regTMA, uint8_t *regTAC, uint8_t *regDIV, Interrupt* interrupts);
    virtual ~Timer();

    void AddCycle();

    void WriteDIV();
    void WriteTAC(uint8_t newValue);

    // Inherited from MemoryByteObserver.
    void AttachToMemorySubject(MemoryByteSubject* subject);
    virtual void UpdateMemoryAddr(uint16_t addr, uint8_t value);

    void PrintTimerData();

    uint16_t GetCounter() {return (*regDIV << 8) | internalCounter;}

    bool SaveState(FILE *file);
    bool LoadState(uint16_t version, FILE *file);

private:
    void ProcessCounterChange(uint16_t oldValue, uint16_t newValue);

    uint8_t *regTIMA;
    uint8_t *regTMA;
    uint8_t *regTAC;
    uint8_t *regDIV;

    uint8_t internalCounter;

    bool regTIMAOverflowed;

    Interrupt* interrupts;
};