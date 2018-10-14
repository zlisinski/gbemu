#pragma once

#include <array>
#include <vector>

#include "gbemu.h"
#include "MemoryByteSubject.h"

enum SpecialRegisters
{
    eRegP1    = 0xFF00, // Joypad

    eRegSB    = 0xFF01, // Serial data
    eRegSC    = 0xFF02, // Serial control

    eRegDIV   = 0xFF04, // Divide register
    eRegTIMA  = 0xFF05, // Timer counter
    eRegTMA   = 0xFF06, // Timer modulo
    eRegTAC   = 0xFF07, // Timer control

    eRegIF    = 0xFF0F, // Interrupt flag

    eRegNR10  = 0xFF10, // Sound mode 1, sweep
    eRegNR11  = 0xFF11, // Sound mode 1, length/wave pattern duty
    eRegNR12  = 0xFF12, // Sound mode 1, envelope
    eRegNR13  = 0xFF13, // Sound mode 1, frequency low
    eRegNR14  = 0xFF14, // Sound mode 1, frequency high
    eRegNR21  = 0xFF16, // Sound mode 2, length/wave pattern duty
    eRegNR22  = 0xFF17, // Sound mode 2, envelope
    eRegNR23  = 0xFF18, // Sound mode 2, frequency low
    eRegNR24  = 0xFF19, // Sound mode 2, frequency high
    eRegNR30  = 0xFF1A, // Sound mode 3, on/off
    eRegNR31  = 0xFF1B, // Sound mode 3, sound length
    eRegNR32  = 0xFF1C, // Sound mode 3, select output level
    eRegNR33  = 0xFF1D, // Sound mode 3, frequency low
    eRegNR34  = 0xFF1E, // Sound mode 3, frequency high
    eRegNR41  = 0xFF20, // Sound mode 4, sound length
    eRegNR42  = 0xFF21, // Sound mode 4, envelope
    eRegNR43  = 0xFF22, // Sound mode 4, polynomial counter
    eRegNR44  = 0xFF23, // Sound mode 4, counter/consecutive
    eRegNR50  = 0xFF24, // Channel control, on/off, volume
    eRegNR51  = 0xFF25, // Sound output terminal
    eRegNR52  = 0xFF26, // Sound on/off
    eRegWave0 = 0xFF30, // Wave pattern RAM
    eRegWave1 = 0xFF31, // Wave pattern RAM
    eRegWave2 = 0xFF32, // Wave pattern RAM
    eRegWave3 = 0xFF33, // Wave pattern RAM
    eRegWave4 = 0xFF34, // Wave pattern RAM
    eRegWave5 = 0xFF35, // Wave pattern RAM
    eRegWave6 = 0xFF36, // Wave pattern RAM
    eRegWave7 = 0xFF37, // Wave pattern RAM
    eRegWave8 = 0xFF38, // Wave pattern RAM
    eRegWave9 = 0xFF39, // Wave pattern RAM
    eRegWaveA = 0xFF3A, // Wave pattern RAM
    eRegWaveB = 0xFF3B, // Wave pattern RAM
    eRegWaveC = 0xFF3C, // Wave pattern RAM
    eRegWaveD = 0xFF3D, // Wave pattern RAM
    eRegWaveE = 0xFF3E, // Wave pattern RAM
    eRegWaveF = 0xFF3F, // Wave pattern RAM

    eRegLCDC  = 0xFF40, // LCD control
    eRegSTAT  = 0xFF41, // LCDC status
    eRegSCY   = 0xFF42, // Scroll Y
    eRegSCX   = 0xFF43, // Scroll X
    eRegLY    = 0xFF44, // LCDC Y coordinate
    eRegLYC   = 0xFF45, // LY compare
    eRegDMA   = 0xFF46, // DMA transfer start address
    eRegBGP   = 0xFF47, // BG & window palette data
    eRegOBP0  = 0xFF48, // Object palette 0 data
    eRegOBP1  = 0xFF49, // Object palette 1 data
    eRegWY    = 0xFF4A, // Window Y position
    eRegWX    = 0xFF4B, // Window X position

    eRegBootDisable = 0xFF50, // Disables boot rom by remapping game rom over boot rom memory

    eRegIE    = 0xFFFF, // Interrupt enable
};

const size_t MEM_SIZE = 0xFFFF;
const size_t ROM_BANK_SIZE = 0x4000;
const size_t BOOT_ROM_SIZE = 0x100;


class Memory : public MemoryByteSubject
{
public:
    Memory();
    virtual ~Memory();

    void SetRomMemory(std::array<uint8_t, BOOT_ROM_SIZE> &bootRomMemory, std::vector<uint8_t> &gameRomMemory);
    void SetRomMemory(std::vector<uint8_t> &gameRomMemory);

    uint8_t ReadByte(uint16_t index) const;
    uint8_t *GetBytePtr(uint16_t index); // Bypasses checking of writes to special addresses. Don't use unless you know what you are doing.
    uint8_t operator[](uint16_t index) const;

    void WriteByte(uint16_t index, uint8_t byte);

    void ClearMemory();

private:
    void DisableBootRom();

    std::array<uint8_t, MEM_SIZE> memory;

    std::array<uint8_t, BOOT_ROM_SIZE> bootRomMemory;
    std::vector<uint8_t> gameRomMemory;
};