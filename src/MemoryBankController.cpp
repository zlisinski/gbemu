#include <sstream>

#include "Logger.h"
#include "MemoryBankController.h"


std::unique_ptr<AbsMbc> MbcFactory::GetMbcInstance(MbcTypes mbcType, MemoryBankInterface *memory)
{
    switch (mbcType)
    {
        case eMbcNone:
            return std::unique_ptr<AbsMbc>(new MbcNone(memory));
        case eMbc1:
            return std::unique_ptr<AbsMbc>(new Mbc1(memory));
        case eMbc2:
            return std::unique_ptr<AbsMbc>(new Mbc2(memory));
        case eMbc3:
            return std::unique_ptr<AbsMbc>(new Mbc3(memory));
        case eMbc5:
            throw NotYetImplementedException("MBC5 mot yet implemented");
        default:
        {
            std::stringstream ss;
            ss << "Unknown MBC type 0x" << std::hex << std::uppercase << (int)mbcType;
            throw NotYetImplementedException(ss.str());
        }
    }
}


AbsMbc::AbsMbc(MemoryBankInterface *memory) :
    memory(memory)
{

}

// MbcNone ////////////////////////////////////////////////////////////////////////////////////////

MbcNone::MbcNone(MemoryBankInterface *memory) :
    AbsMbc(memory)
{

}


void MbcNone::WriteByte(uint16_t addr, uint8_t byte)
{
    (void)addr; // Stop warnings about unused variables.
    (void)byte; // Stop warnings about unused variables.
}


bool MbcNone::SaveState(FILE *file)
{
    (void)file; // Stop warnings about unused variables.
    return true;
}


bool MbcNone::LoadState(FILE *file)
{
    (void)file; // Stop warnings about unused variables.
    return true;
}


// Mbc1 ///////////////////////////////////////////////////////////////////////////////////////////


Mbc1::Mbc1(MemoryBankInterface *memory) :
    AbsMbc(memory),
    regRamEnable(0),
    regRomLowBits(1),
    regRomHighBits(0),
    regRamMode(0)
{

}


void Mbc1::WriteByte(uint16_t addr, uint8_t byte)
{
    switch (addr & 0xE000)
    {
        case 0x0000: // 0x0000 - 0x1FFF
            regRamEnable = byte & 0x0F;
            LogInstruction("Writing %02X(%02X) to %04X", byte, regRamEnable, addr);
            memory->EnableRam(regRamEnable == 0x0A);
            return;

        case 0x2000: // 0x2000 - 0x3FFF
            regRomLowBits = byte & 0x1F;
            LogInstruction("Writing %02X(%02X) to %04X", byte, regRomLowBits, addr);

            // Low bits can't be zero. Bank 0x00/0x20/0x40/0x60 become 0x01/0x21/0x41/0x61.
            if (regRomLowBits == 0)
                regRomLowBits = 1;

            memory->MapRomBank(regRomLowBits + (regRomHighBits * 0x20 * !regRamMode));
            break;

        case 0x4000: // 0x4000 - 0x5FFF
            regRomHighBits = byte & 0x03;
            LogInstruction("Writing %02X(%02X) to %04X", byte, regRomHighBits, addr);

            if (regRamMode)
                memory->MapRamBank(regRomHighBits);
            else
                memory->MapRomBank(regRomLowBits + (regRomHighBits * 0x20));
            break;

        case 0x6000: // 0x6000 - 0x7FFF
            regRamMode = byte & 0x01;
            LogInstruction("Writing %02X(%02X) to %04X", byte, regRamMode, addr);
            if (regRamMode)
            {
                // Register2 is now used for RAM, so remap ROM without top two bits.
                memory->MapRomBank(regRomLowBits);
            }
            else
            {
                // Multiple RAM banks have been turned off, remap to bank 0.
                memory->MapRamBank(0);
            }
            break;

        default:
            LogError("Writing %02X to %04X ignored by MBC1", byte, addr);
            break;
    }
}


bool Mbc1::SaveState(FILE *file)
{
    if (!fwrite(&regRamEnable, sizeof(regRamEnable), 1, file))
        return false;

    if (!fwrite(&regRomLowBits, sizeof(regRomLowBits), 1, file))
        return false;

    if (!fwrite(&regRomHighBits, sizeof(regRomHighBits), 1, file))
        return false;

    if (!fwrite(&regRamMode, sizeof(regRamMode), 1, file))
        return false;

    return true;
}


bool Mbc1::LoadState(FILE *file)
{
    if (!fread(&regRamEnable, sizeof(regRamEnable), 1, file))
        return false;

    if (!fread(&regRomLowBits, sizeof(regRomLowBits), 1, file))
        return false;

    if (!fread(&regRomHighBits, sizeof(regRomHighBits), 1, file))
        return false;

    if (!fread(&regRamMode, sizeof(regRamMode), 1, file))
        return false;

    return true;
}


// Mbc2 ///////////////////////////////////////////////////////////////////////////////////////////


Mbc2::Mbc2(MemoryBankInterface *memory) :
    AbsMbc(memory),
    regRamEnable(0),
    regRomBank(1)
{

}


void Mbc2::WriteByte(uint16_t addr, uint8_t byte)
{
    switch (addr & 0xE100)
    {
        case 0x0000: // 0x0000 - 0x1FFF, except values where low bit of high byte is 1 (0x01nn, 0x03nn, 0x05nn, etc)
            regRamEnable = byte & 0x0F;
            LogInstruction("Writing %02X(%02X) to %04X", byte, regRamEnable, addr);
            memory->EnableRam(regRamEnable == 0x0A);
            break;
        case 0x2100: // 0x2000 - 0x3FFF, except values where low bit of high byte is 0 (0x20nn, 0x22nn, 0x24nn, etc).
            regRomBank = byte & 0x0F;
            LogInstruction("Writing %02X(%02X) to %04X", byte, regRomBank, addr);

            // Can't select bank 0.
            if (regRomBank == 0)
                regRomBank = 1;

            memory->MapRomBank(regRomBank);
            break;
        default:
            LogError("Writing %02X to %04X ignored by MBC2", byte, addr);
            break;
    }
}


bool Mbc2::SaveState(FILE *file)
{
    if (!fwrite(&regRamEnable, sizeof(regRamEnable), 1, file))
        return false;

    if (!fwrite(&regRomBank, sizeof(regRomBank), 1, file))
        return false;

    return true;
}


bool Mbc2::LoadState(FILE *file)
{
    if (!fread(&regRamEnable, sizeof(regRamEnable), 1, file))
        return false;

    if (!fread(&regRomBank, sizeof(regRomBank), 1, file))
        return false;

    return true;
}


// Mbc3 ///////////////////////////////////////////////////////////////////////////////////////////


Mbc3::Mbc3(MemoryBankInterface *memory) :
    AbsMbc(memory),
    regRamEnable(0),
    regRomBank(1),
    regRamBank(0),
    regRtcLatch(0)
{

}


void Mbc3::WriteByte(uint16_t addr, uint8_t byte)
{
    switch (addr & 0xE000)
    {
        case 0x0000: // 0x0000 - 0x1FFF
            regRamEnable = byte & 0x0F;
            LogInstruction("Writing %02X(%02X) to %04X", byte, regRamEnable, addr);
            memory->EnableRam(regRamEnable == 0x0A);
            return;

        case 0x2000: // 0x2000 - 0x3FFF
            regRomBank = byte & 0x7F;
            LogInstruction("Writing %02X(%02X) to %04X", byte, regRomBank, addr);

            // Low bits can't be zero. Bank 0x00/0x20/0x40/0x60 become 0x01/0x21/0x41/0x61.
            if (regRomBank == 0)
                regRomBank = 1;

            memory->MapRomBank(regRomBank);
            break;

        case 0x4000: // 0x4000 - 0x5FFF
            // Values 0x08 - 0x0C are for RTC, which is NYI.
            regRamBank = byte & 0x03;
            LogInstruction("Writing %02X(%02X) to %04X", byte, regRamBank, addr);

            memory->MapRamBank(regRamBank);
            break;

        case 0x6000: // 0x6000 - 0x7FFF
            // RTC is not yet implemented.
            break;

        default:
            LogError("Writing %02X to %04X ignored by MBC3", byte, addr);
            break;
    }
}


bool Mbc3::SaveState(FILE *file)
{
    if (!fwrite(&regRamEnable, sizeof(regRamEnable), 1, file))
        return false;

    if (!fwrite(&regRomBank, sizeof(regRomBank), 1, file))
        return false;

    if (!fwrite(&regRamBank, sizeof(regRamBank), 1, file))
        return false;

    if (!fwrite(&regRtcLatch, sizeof(regRtcLatch), 1, file))
        return false;

    return true;
}


bool Mbc3::LoadState(FILE *file)
{
    if (!fread(&regRamEnable, sizeof(regRamEnable), 1, file))
        return false;

    if (!fread(&regRomBank, sizeof(regRomBank), 1, file))
        return false;

    if (!fread(&regRamBank, sizeof(regRamBank), 1, file))
        return false;

    if (!fread(&regRtcLatch, sizeof(regRtcLatch), 1, file))
        return false;

    return true;
}