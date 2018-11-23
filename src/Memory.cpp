#include <algorithm>
#include <sstream>

#include "Memory.h"

Memory::Memory() :
    isDmaActive(false),
    dmaOffset(0)
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


void Memory::AttachToTimerSubject(std::shared_ptr<TimerSubject> subject)
{
    this->timerSubject = subject;
    subject->AttachObserver(shared_from_this());
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