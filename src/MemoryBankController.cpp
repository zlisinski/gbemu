#include <sstream>

#include "MemoryBankController.h"

MemoryBankController::MemoryBankController(MbcTypes mbcType, uint8_t romBankCount, uint8_t ramBankCount, MemoryBankInterface *memory) :
    mbcType(mbcType),
    romBankCount(romBankCount),
    ramBankCount(ramBankCount),
    memory(memory),
    register0(0),
    register1(0),
    register2(0),
    register3(0)
{

}


MemoryBankController::~MemoryBankController()
{

}


void MemoryBankController::WriteByte(uint16_t addr, uint8_t byte)
{
    switch (mbcType)
    {
        case eMbcNone:
            break;
        case eMbc1:
            if (addr >= 0 && addr <= 0x1FFF)
            {
                register0 = byte & 0x0F;
                memory->EnableRam(register0 == 0x0A);
            }
            else if (addr >= 0x2000 && addr <= 0x3FFF)
            {
                register1 = byte & 0x1F;

                // Can't select bank 0.
                if (register1 == 0)
                    register1 = 1;

                memory->MapRomBank(register1 + (register2 * 0x20 * !register3));
            }
            else if (addr >= 0x4000 && addr <= 0x5FFF)
            {
                register2 = byte & 0x03;

                if (register3)
                    memory->MapRamBank(register2);
                else
                    memory->MapRomBank(register1 + (register2 * 0x20));
            }
            else if (addr >= 0x6000 && addr <= 0x7FFF)
            {
                register3 = byte & 0x01;
            }
            break;
        case eMbc2:
            if (addr >= 0 && addr <= 0x0FFF)
            {
                register0 = byte & 0x0F;
                memory->EnableRam(register0 == 0x0A);
            }
            else if (addr >= 0x2000 && addr <= 0x3FFF)
            {
                register1 = byte & 0x0F;

                // Can't select bank 0.
                if (register1 == 0)
                    register1 = 1;

                memory->MapRomBank(register1);
            }
            break;
        case eMbc3:
            throw NotYetImplementedException("MBC3 mot yet implemented");
            break;
        case eMbc5:
            throw NotYetImplementedException("MBC5 mot yet implemented");
            break;
        default:
            {
                std::stringstream ss;
                ss << "Unknown MBC type 0x" << std::hex << std::uppercase << (int)mbcType;
                throw NotYetImplementedException(ss.str());
            }
            break;
    }
}