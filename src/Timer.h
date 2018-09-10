#pragma once

#include <memory>

#include "gbemu.h"
#include "Memory.h"
#include "MemoryByteObserver.h"
#include "MemoryByteSubject.h"

// The memory for the timer registers belongs to the Memory class, but the Timer class 'owns' the access to the memory.
// Because of this, we use direct pointers to the memory, and any writes that happen through the Memory class will get
// sent to this class for processing.
class Timer : public MemoryByteObserver, public std::enable_shared_from_this<Timer>
{
public:
    //Timer(uint8_t *regTIMA, uint8_t *regTMA, uint8_t *regTAC, uint8_t *regDIV, MemoryByteSubject &subject);
    Timer(std::shared_ptr<Memory> memory);
    virtual ~Timer();

    void AttachToSubject();

    void ProcessCycles(uint cycles);

    void WriteDIV();
    void WriteTAC(uint8_t newValue);

    // Inherited from MemoryByteObserver.
    virtual void UpdateMemoryAddr(uint16_t addr, uint8_t value);

    void PrintTimerData();

private:
    void ProcessCounterChange(uint16_t oldValue, uint16_t newValue);

    // Hold a pointer to 'memory' to insure that reg pointers don't get deleted while we're using them.
    std::shared_ptr<Memory> memory;

    uint8_t *regTIMA;
    uint8_t *regTMA;
    uint8_t *regTAC;
    uint8_t *regDIV;

    uint8_t internalCounter;

    bool regTIMAOverflowed;  
};