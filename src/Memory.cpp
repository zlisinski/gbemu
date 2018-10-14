#include <algorithm>
#include <sstream>

#include "Memory.h"

Memory::Memory()
{
    
}


Memory::~Memory()
{

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
}


void Memory::SetRomMemory(std::vector<uint8_t> &gameRomMemory)
{
    // Makes a copy. Should I do this?
    this->gameRomMemory = gameRomMemory;

    size_t size = std::min(gameRomMemory.size(), ROM_BANK_SIZE * 2);

    memcpy(&memory[0], &gameRomMemory[0], size);
}


uint8_t Memory::ReadByte(uint16_t index) const
{
    // Reads from 0xFEA0-0xFEFF return 0x00.
    if (index >= 0xFEA0 && index <= 0xFEFF)
        return 0x00;

    // Reads from 0xE000-0xFDFF mirror 0xC000-0xDDFF.
    if (index >= 0xE000 && index <= 0xFDFF)
        return memory[index - 0x2000];

    // Stub out the joypad for now.
    if (index == eRegP1)
        return 0xFF;

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
        case eRegSC: // 0xFF02
            if (byte & 0x80)
            {
                FILE *file = fopen("serial.txt", "a");
                fputc(memory[eRegSB], file);
                fclose(file);
                DBG("Serial: %02X, '%c'\n", memory[eRegSB], memory[eRegSB]);

                memory[index] = byte;
            }
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

    // Let observers handle the update. If there are no observers for this address, update the value.
    if (!NotifyObservers(index, byte))
    {
        memory[index] = byte;
    }
}


void Memory::ClearMemory()
{
    memory.fill(0);
}


void Memory::DisableBootRom()
{
    memcpy(memory.data(), gameRomMemory.data(), BOOT_ROM_SIZE);
}