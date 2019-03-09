#pragma once

#include "gbemu.h"
#include "MemoryByteObserver.h"
#include "MemoryByteSubject.h"


enum eInterruptTypes
{
    eIntVBlank = 0,
    eIntLCDC   = 1,
    eIntTimer  = 2,
    eIntSerial = 3,
    eIntJoypad = 4
};

enum eInterruptBits
{
    eIntBitVBlank = 0x01,
    eIntBitLCDC   = 0x02,
    eIntBitTimer  = 0x04,
    eIntBitSerial = 0x08,
    eIntBitJoypad = 0x10
};


class Interrupt : public MemoryByteObserver
{
public:
    Interrupt(uint8_t *regIE, uint8_t *regIF);
    virtual ~Interrupt();

    bool CheckInterrupts(eInterruptTypes &intType);

    bool Enabled() {return flagIME;}
    void SetEnabled(bool value) {flagIME = value;}

    uint16_t GetInterruptAddress(eInterruptTypes type);

    void RequestInterrupt(eInterruptTypes type);
    void ClearInterrupt(eInterruptTypes type);

    bool SaveState(FILE *file);
    bool LoadState(FILE *file);
    
    // Inherited from MemoryByteObserver.
    virtual void AttachToMemorySubject(MemoryByteSubject* subject);
    virtual void UpdateMemoryAddr(uint16_t addr, uint8_t value);

private:
    uint8_t *regIE;
    uint8_t *regIF;
    bool flagIME;
};