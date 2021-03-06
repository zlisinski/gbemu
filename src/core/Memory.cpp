#include <algorithm>
#include <memory>
#include <sstream>
#include <string.h>
#include <unordered_map>
#include <unordered_set>

#include "DebuggerInterface.h"
#include "InfoInterface.h"
#include "Logger.h"
#include "Memory.h"

const uint16_t MbcTypeOffset = 0x0147;
const uint16_t RomSizeOffset = 0x0148;
const uint16_t RamSizeOffset = 0x0149;


const std::unordered_map<uint8_t, MbcTypes> MbcTypesMap = {
    {0x00, eMbcNone}, // ROM only
    {0x01, eMbc1},    // MBC1
    {0x02, eMbc1},    // MBC1 + RAM
    {0x03, eMbc1},    // MBC1 + RAM + BAT
    {0x05, eMbc2},    // MBC2
    {0x06, eMbc2},    // MBC2 + BAT
    {0x08, eMbcNone}, // RAM
    {0x09, eMbcNone}, // RAM + BAT
    //{0x0F, eMbc3},  // MBC3 + TIMER + BAT
    //{0x10, eMbc3},  // MBC3 + TIMER + RAM + BAT
    {0x11, eMbc3},    // MBC3
    {0x12, eMbc3},    // MBC3 + RAM
    {0x13, eMbc3},    // MBC3 + RAM + BAT
    {0x19, eMbc5},    // MBC5
    {0x1A, eMbc5},    // MBC5 + RAM
    {0x1B, eMbc5}     // MBC5 + RAM + BAT
};

const std::unordered_set<uint8_t> BatteryBackedRamSet = {
    0x03, // MBC1
    0x06, // MBC2
    0x09, // No MBC
    0x0F, // MBC3 + Timer
    0x10, // MBC3 + Timer
    0x13, // MBC3
    0x1B  // MBC5
};

const std::unordered_map<uint8_t, uint8_t> RomBankCountMap = {
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

const std::unordered_map<uint8_t, uint8_t> RamBankCountMap = {
    {0, 0},  // None
    {1, 1},  // 16Kb,  2KB
    {2, 1},  // 64Kb,  8KB
    {3, 4},  // 256Kb, 32KB
    {4, 16}, // 1Mb,   128KB
};


Memory::Memory(InfoInterface *infoInterface, DebuggerInterface *debuggerInterface) :
    isDmaActive(false),
    dmaOffset(0),
    mbcType(eMbcNone),
    romBankCount(0),
    ramBankCount(0),
    curRomBank(1),
    curRamBank(0),
    batteryBackedRam(false),
    ramEnabled(false),
    infoInterface(infoInterface),
    debuggerInterface(debuggerInterface)
{
    ClearMemory();
}


Memory::~Memory()
{

}


void Memory::SetRomMemory(std::vector<uint8_t> &bootRomMemory, std::vector<uint8_t> &gameRomMemory)
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

    mbc = MbcFactory::GetMbcInstance(mbcType, this);

    ramBanks.resize(ramBankCount * RAM_BANK_SIZE);
}


void Memory::SetRomMemory(std::vector<uint8_t> &gameRomMemory)
{
    // Makes a copy. Should I do this?
    this->gameRomMemory = gameRomMemory;

    size_t size = std::min(gameRomMemory.size(), ROM_BANK_SIZE * 2);

    memcpy(&memory[0], &gameRomMemory[0], size);

    CheckRom();

    mbc = MbcFactory::GetMbcInstance(mbcType, this);

    ramBanks.resize(ramBankCount * RAM_BANK_SIZE);
}


uint8_t Memory::ReadByte(uint16_t index) const
{
    // Unused IO registers return 0xFF.
    switch (index)
    {
        case 0xFF03:
        case 0xFF08: case 0xFF09: case 0xFF0A: case 0xFF0B: case 0xFF0C: case 0xFF0D: case 0xFF0E:
        case 0xFF15:
        case 0xFF1F:
        case 0xFF27: case 0xFF28: case 0xFF29:
        case 0xFF4C: case 0xFF4D: case 0xFF4E: case 0xFF4F:
        case 0xFF50: case 0xFF51: case 0xFF52: case 0xFF53: case 0xFF54: case 0xFF55: case 0xFF56: case 0xFF57:
        case 0xFF58: case 0xFF59: case 0xFF5A: case 0xFF5B: case 0xFF5C: case 0xFF5D: case 0xFF5E: case 0xFF5F:
        case 0xFF60: case 0xFF61: case 0xFF62: case 0xFF63: case 0xFF64: case 0xFF65: case 0xFF66: case 0xFF67:
        case 0xFF68: case 0xFF69: case 0xFF6A: case 0xFF6B: case 0xFF6C: case 0xFF6D: case 0xFF6E: case 0xFF6F:
        case 0xFF70: case 0xFF71: case 0xFF72: case 0xFF73: case 0xFF74: case 0xFF75: case 0xFF76: case 0xFF77:
        case 0xFF78: case 0xFF79: case 0xFF7A: case 0xFF7B: case 0xFF7C: case 0xFF7D: case 0xFF7E: case 0xFF7F:
            return 0xFF;
    }

    // Reads from 0xFEA0-0xFEFF return 0x00.
    if (index >= 0xFEA0 && index <= 0xFEFF)
        return 0x00;

    // Reads from 0xE000-0xFDFF mirror 0xC000-0xDDFF.
    if (index >= 0xE000 && index <= 0xFDFF)
        return memory[index - 0x2000];

    // Reads from SRAM return 0xFF when not enabled.
    if (index >= 0xA000 && index < 0xC000 && ramEnabled == false)
        return 0xFF;

    if (HasIoRegisterProxy(index))
        return ReadIoRegisterProxy(index);

    return memory[index];
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
        LogInstruction("Write to ROM area 0x%04X value 0x%02X", index, byte);
        if (mbc)
            mbc->WriteByte(index, byte);

        return;
    }

    // Ignore writes to SRAM when not enabled.
    if (index >= 0xA000 && index < 0xC000 && ramEnabled == false)
        return;

    if (debuggerInterface != NULL)
        debuggerInterface->MemoryChanged(index, 1);

    // Let observers handle the update. If there are no observers for this address, update the value.
    if (!WriteIoRegisterProxy(index, byte))
    {
        memory[index] = byte;
    }
}


void Memory::ClearMemory()
{
    memory.fill(0);
    ramBanks.clear();
}


void Memory::LoadRam(const std::string &filename)
{
    if (batteryBackedRam == false || ramBankCount == 0)
        return;

    FILE *file = fopen(filename.c_str(), "rb");
    if (file == NULL)
    {
        printf("Error opening RAM file %s\n", filename.c_str());
        return;
    }

    if (mbcType == eMbc2)
    {
        // MBC2 has 512 * 4 bits of RAM.
        fread(&memory[SWITCHABLE_RAM_BANK_OFFSET], 1, 512, file);
    }
    else
    {
        fread(&ramBanks[0], 1, ramBanks.size(), file);

        // Load the current ram bank into SRAM.
        memcpy(&memory[SWITCHABLE_RAM_BANK_OFFSET], &ramBanks[curRamBank * RAM_BANK_SIZE], RAM_BANK_SIZE);
    }

    fclose(file);
}


void Memory::SaveRam(const std::string &filename)
{
    if (batteryBackedRam == false || ramBankCount == 0)
        return;

    FILE *file = fopen(filename.c_str(), "wb");
    if (file == NULL)
    {
        printf("Error opening RAM file %s\n", filename.c_str());
        return;
    }

    if (mbcType == eMbc2)
    {
        // MBC2 has 512 * 4 bits of RAM.
        fwrite(&memory[SWITCHABLE_RAM_BANK_OFFSET], 1, 512, file);
    }
    else
    {
        // Sync current SRAM bank into banks array.
        memcpy(&ramBanks[curRamBank * RAM_BANK_SIZE], &memory[SWITCHABLE_RAM_BANK_OFFSET], RAM_BANK_SIZE);

        fwrite(&ramBanks[0], 1, ramBanks.size(), file);
    }

    fclose(file);
}


bool Memory::SaveState(FILE *file)
{
    if (!fwrite(&memory[0], MEM_SIZE, 1, file))
        return false;

    // If there is only a single RAM bank, it lives completely inside the main memory array.
    if (ramBankCount > 1)
    {
        if (!fwrite(&ramBanks[0], ramBanks.size(), 1, file))
            return false;
    }

    if (!fwrite(&curRomBank, sizeof(curRomBank), 1, file))
        return false;

    if (!fwrite(&curRamBank, sizeof(curRamBank), 1, file))
        return false;

    if (!fwrite(&ramEnabled, sizeof(ramEnabled), 1, file))
        return false;

    if (!fwrite(&isDmaActive, sizeof(isDmaActive), 1, file))
        return false;

    if (!fwrite(&dmaOffset, sizeof(dmaOffset), 1, file))
        return false;

    return mbc->SaveState(file);
}


bool Memory::LoadState(uint16_t version, FILE *file)
{
    if (!fread(&memory[0], MEM_SIZE, 1, file))
        return false;

    // If there is only a single RAM bank, it lives completely inside the main memory array.
    if (ramBankCount > 1)
    {
        if (!fread(&ramBanks[0], ramBanks.size(), 1, file))
            return false;
    }

    if (version == 1)
    {
        // Set defaults for values not in save version 1.
        // Will this break things? I don't know.
        curRomBank = 0;
        curRamBank = 0;
        ramEnabled = false;
    }
    else
    {
        if (!fread(&curRomBank, sizeof(curRomBank), 1, file))
            return false;

        if (!fread(&curRamBank, sizeof(curRamBank), 1, file))
            return false;

        if (!fread(&ramEnabled, sizeof(ramEnabled), 1, file))
            return false;
    }

    if (!fread(&isDmaActive, sizeof(isDmaActive), 1, file))
        return false;

    if (!fread(&dmaOffset, sizeof(dmaOffset), 1, file))
        return false;

    return mbc->LoadState(version, file);
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

    if (debuggerInterface != NULL)
        debuggerInterface->MemoryChanged(0, BOOT_ROM_SIZE);
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
    LogInfo("ROM size = %02X", romBankCount);

    // Check for valid RAM size.
    it = RamBankCountMap.find(memory[RamSizeOffset]);
    if (it == RamBankCountMap.end())
    {
        std::stringstream ss;
        ss << "Ram size invalid or not yet implemented: " << std::hex << (int)memory[RamSizeOffset];
        throw NotYetImplementedException(ss.str());
    }
    ramBankCount = it->second;
    LogInfo("RAM size = %02X", ramBankCount);

    // Check Memory Bank Controller.
    auto mbcIt = MbcTypesMap.find(memory[MbcTypeOffset]);
    if (mbcIt == MbcTypesMap.end())
    {
        std::stringstream ss;
        ss << "MBC value not yet implemented: " << std::hex << (int)memory[MbcTypeOffset];
        throw NotYetImplementedException(ss.str());
    }
    mbcType = mbcIt->second;
    LogInfo("MBC type = %02X", mbcType);

    // MBC2 has 512 * 4 bits of RAM, even though the RAM bank count is 0.
    if (mbcType == eMbc2)
        ramBankCount = 1;

    // Get battery backed ram.
    batteryBackedRam = BatteryBackedRamSet.count(memory[MbcTypeOffset]);

    if (infoInterface)
    {
        infoInterface->SetMbcType(mbcType);
        infoInterface->SetRomBanks(romBankCount);
        infoInterface->SetRamBanks(ramBankCount);
        infoInterface->SetBatteryBackedRam(batteryBackedRam);
    }
}


void Memory::MapRomBank(uint8_t bank)
{
    if (bank >= romBankCount)
    {
        LogWarning("Asking for ROM bank 0x%02X when ROM bank count is 0x%02X", bank, romBankCount);

        bank = bank % romBankCount;
    }

    curRomBank = bank;

    LogInstruction("Copying ROM bank 0x%02X", bank);
    if (infoInterface)
        infoInterface->SetMappedRomBank(bank);

    memcpy(&memory[SWITCHABLE_ROM_BANK_OFFSET], &gameRomMemory[bank * ROM_BANK_SIZE], ROM_BANK_SIZE);

    if (debuggerInterface != NULL)
        debuggerInterface->MemoryChanged(SWITCHABLE_ROM_BANK_OFFSET, ROM_BANK_SIZE);
}


void Memory::MapRamBank(uint8_t bank)
{
    // One of mooneye's tests does this.
    if (ramBankCount == 0)
        return;

    if (bank >= ramBankCount)
    {
        LogWarning("Asking for RAM bank 0x%02X when RAM bank count is 0x%02X", bank, ramBankCount);

        bank = bank % ramBankCount;
        //throw std::range_error("Invalid RAM bank");
    }

    // Copy from current SRAM into old bank.
    memcpy(&ramBanks[curRamBank * RAM_BANK_SIZE], &memory[SWITCHABLE_RAM_BANK_OFFSET], RAM_BANK_SIZE);

    curRamBank = bank;

    if (infoInterface)
        infoInterface->SetMappedRamBank(bank);

    // Copy new bank into SRAM.
    memcpy(&memory[SWITCHABLE_RAM_BANK_OFFSET], &ramBanks[curRamBank * RAM_BANK_SIZE], RAM_BANK_SIZE);

    if (debuggerInterface != NULL)
        debuggerInterface->MemoryChanged(SWITCHABLE_RAM_BANK_OFFSET, RAM_BANK_SIZE);
}


void Memory::EnableRam(bool enable)
{
    LogInfo("Enable RAM = %s", enable ? "true" : "false");
    ramEnabled = enable;
}