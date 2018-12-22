#include <algorithm>
#include <map>
#include <sstream>

#include "Memory.h"

const uint16_t MbcTypeOffset = 0x0147;
const uint16_t RomSizeOffset = 0x0148;
const uint16_t RamSizeOffset = 0x0149;


const std::map<uint8_t, MbcTypes> MbcTypesMap = {
    {0x00, eMbcNone}, // ROM only
    {0x01, eMbc1},    // MBC1
    {0x02, eMbc1},    // MBC1 + RAM
    {0x03, eMbc1},    // MBC1 + RAM + BAT
    {0x05, eMbc2},    // MBC2
    {0x06, eMbc2},    // MBC2 + BAT
    {0x08, eMbcNone}, // RAM
    {0x09, eMbcNone}, // RAM + BAT
    {0x12, eMbc3},    // MBC3 + RAM
    {0x13, eMbc3},    // MBC3 + RAM + BAT
    {0x19, eMbc5},    // MBC5
    {0x1A, eMbc5},    // MBC5 + RAM
    {0x1B, eMbc5}     // MBC5 + RAM + BAT
};

const std::map<uint8_t, uint8_t> RomBankCountMap = {
    {0, 2},     // 256Kb, 32KB
    {1, 4},     // 512Kb, 64KB
    {2, 8},     // 1Mb,   128KB
    {3, 16},    // 2Mb,   256KB
    {4, 32},    // 4Mb,   512KB
    {5, 64},    // 8Mb,   1MB
    {6, 128},   // 16Mb,  2MB
    {0x52, 72}, // 9Mb,   1.1MB
    {0x53, 80}, // 10Mb,  1.2MB
    {0x54, 96}  // 12MB,  1.5MB
};

const std::map<uint8_t, uint8_t> RamBankCountMap = {
    {0, 0},  // None
    {1, 1},  // 16Kb,  2KB
    {2, 1},  // 64Kb,  8KB
    {3, 4},  // 256Kb, 32KB
    {4, 16}, // 1Mb,   128KB
};


Memory::Memory() :
    isDmaActive(false),
    dmaOffset(0),
    mbcType(eMbcNone),
    romBankCount(0),
    ramBankCount(0)
{
    ClearMemory();
}


Memory::~Memory()
{
    if (timerSubject)
        timerSubject->DetachObserver(this);
}


void Memory::SetRomMemory(std::array<uint8_t, BOOT_ROM_SIZE> &bootRomMemory, std::vector<uint8_t> &gameRomMemory)
{
    // Makes a copy. Should I do this?
    this->bootRomMemory = bootRomMemory;
    this->gameRomMemory = gameRomMemory;

    memcpy(memory.data(), bootRomMemory.data(), BOOT_ROM_SIZE);

    if (gameRomMemory.size() <= BOOT_ROM_SIZE)
    {
        std::stringstream ss;
        ss << "Size of gameRomMemory(" << gameRomMemory.size() << ") is less than " << BOOT_ROM_SIZE;
        throw std::range_error(ss.str());
    }

    size_t size = std::min(gameRomMemory.size() - BOOT_ROM_SIZE, (ROM_BANK_SIZE * 2) - BOOT_ROM_SIZE);

    memcpy(&memory[BOOT_ROM_SIZE], &gameRomMemory[BOOT_ROM_SIZE], size);

    CheckRom();

    mbc = std::unique_ptr<MemoryBankController>(new MemoryBankController(mbcType, romBankCount, ramBankCount, this));
}


void Memory::SetRomMemory(std::vector<uint8_t> &gameRomMemory)
{
    // Makes a copy. Should I do this?
    this->gameRomMemory = gameRomMemory;

    size_t size = std::min(gameRomMemory.size(), ROM_BANK_SIZE * 2);

    memcpy(&memory[0], &gameRomMemory[0], size);

    CheckRom();

    mbc = std::unique_ptr<MemoryBankController>(new MemoryBankController(mbcType, romBankCount, ramBankCount, this));
}


uint8_t Memory::ReadByte(uint16_t index) const
{
    // Reads from 0xFEA0-0xFEFF return 0x00.
    if (index >= 0xFEA0 && index <= 0xFEFF)
        return 0x00;

    // Reads from 0xE000-0xFDFF mirror 0xC000-0xDDFF.
    if (index >= 0xE000 && index <= 0xFDFF)
        return memory[index - 0x2000];

    if (index == eRegSC) // Unused regSC bits are always 1.
        return memory[index] | 0x7E;

    return memory[index];
}


uint8_t *Memory::GetBytePtr(uint16_t index)
{
    return &memory[index];
}


uint8_t Memory::operator[](uint16_t index) const
{
    return ReadByte(index);
}


void Memory::WriteByte(uint16_t index, uint8_t byte)
{
    switch (index)
    {
        case eRegDMA: // 0xFF46
            // Start a DMA transfer next cycle, if one is already active, start over at the beginning.
            memory[eRegDMA] = byte;
            dmaOffset = 0;
            isDmaActive = true;
            return;

        case eRegBootDisable: // 0xFF50
            if (byte != 0)
                DisableBootRom();
            return;
    }

    // If we get here, it wasn't handled by the switch.

    // Ignore writes to 0xFEA0-0xFEFF
    if (index >= 0xFEA0 && index <= 0xFEFF)
        return;

    // Writes to 0xE000-0xFDFF get mirrored to 0xC000-0xDDFF.
    if (index >= 0xE000 && index <= 0xFDFF)
        index -= 0x2000;

    // Writes to ROM area get mapped to MBC.
    if (index < 0x8000)
    {
        DBG("Write to ROM area 0x%04X value 0x%02X\n", index, byte);
        if (mbc)
            mbc->WriteByte(index, byte);
    }

    // Let observers handle the update. If there are no observers for this address, update the value.
    if (!NotifyObservers(index, byte))
    {
        memory[index] = byte;
    }
}


void Memory::ClearMemory()
{
    memory.fill(0);
    ramBanks.clear();
}


void Memory::AttachToTimerSubject(TimerSubject* subject)
{
    this->timerSubject = subject;
    subject->AttachObserver(this);
}


void Memory::UpdateTimer(uint value)
{
    // Return if we're not doing a DMA transfer.
    if (!isDmaActive)
        return;

    for (uint i = 0; i < value / 4; i++)
    {
        if (dmaOffset >= OAM_RAM_LEN)
        {
            // End DMA when end is reached.
            isDmaActive = false;
            dmaOffset = 0;
            break;
        }

        uint16_t srcAddr = (memory[eRegDMA] << 8) | dmaOffset;
        uint16_t destAddr = OAM_RAM_START | dmaOffset;
        uint8_t byte = ReadByte(srcAddr); // Call ReadByte() in case we're reading from a special address.
        memory[destAddr] = byte; // Write directly to memory, since there is no special processing to do on OAM memory.

        dmaOffset++;
    }
}


void Memory::DisableBootRom()
{
    // TODO: Fix this segfaulting when gameRomMemory isn't set.
    memcpy(memory.data(), gameRomMemory.data(), BOOT_ROM_SIZE);
}


void Memory::CheckRom()
{
    // Check for valid ROM size.
    auto it = RomBankCountMap.find(memory[RomSizeOffset]);
    if (it == RomBankCountMap.end())
    {
        std::stringstream ss;
        ss << "Rom size invalid or not yet implemented: " << std::hex << (int)memory[RomSizeOffset];
        throw NotYetImplementedException(ss.str());
    }
    romBankCount = it->second;
    printf("ROM size = %02X\n", romBankCount);

    // Check for valid RAM size.
    it = RamBankCountMap.find(memory[RamSizeOffset]);
    if (it == RamBankCountMap.end())
    {
        std::stringstream ss;
        ss << "Ram size invalid or not yet implemented: " << std::hex << (int)memory[RamSizeOffset];
        throw NotYetImplementedException(ss.str());
    }
    ramBankCount = it->second;
    printf("RAM size = %02X\n", ramBankCount);

    // Check Memory Bank Controller.
    auto mbcIt = MbcTypesMap.find(memory[MbcTypeOffset]);
    if (mbcIt == MbcTypesMap.end())
    {
        std::stringstream ss;
        ss << "MBC value not yet implemented: " << std::hex << (int)memory[MbcTypeOffset];
        throw NotYetImplementedException(ss.str());
    }
    mbcType = mbcIt->second;
    printf("MBC type = %02X\n", mbcType);
}


void Memory::MapRomBank(uint bank)
{
    if (bank > romBankCount)
    {
        std::stringstream ss;
        ss << "Asking for ROM bank 0x" << std::hex << std::uppercase << (int)bank << " when ROM bank count is 0x" << (int)romBankCount;
        printf("%s\n", ss.str().c_str());

        bank = bank % romBankCount;
        //throw std::range_error(ss.str());
    }

    //printf("Copying ROM bank 0x%02X\n", bank);

    memcpy(&memory[SWITCHABLE_ROM_BANK_OFFSET], &gameRomMemory[bank * ROM_BANK_SIZE], ROM_BANK_SIZE);
}


void Memory::MapRamBank(uint bank)
{

}


void Memory::EnableRam(bool enable)
{
    printf("Enable RAM = %s\n", enable ? "true" : "false");
}