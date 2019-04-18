#pragma once

#include "gbemu.h"
#include "IoRegisterProxy.h"


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


class Interrupt : public IoRegisterProxy
{
public:
    Interrupt(IoRegisterSubject *ioRegisterSubject);
    virtual ~Interrupt();

    bool CheckInterrupts(eInterruptTypes &intType);

    bool Enabled() {return flagIME;}
    void SetEnabled(bool value) {flagIME = value;}

    uint16_t GetInterruptAddress(eInterruptTypes type);

    void RequestInterrupt(eInterruptTypes type);
    void ClearInterrupt(eInterruptTypes type);

    bool SaveState(FILE *file);
    bool LoadState(uint16_t version, FILE *file);

    // Inherited from IoRegisterProxy.
    virtual bool WriteByte(uint16_t address, uint8_t byte);
    virtual uint8_t ReadByte(uint16_t address) const;

private:
    uint8_t *regIE;
    uint8_t *regIF;
    bool flagIME;
};