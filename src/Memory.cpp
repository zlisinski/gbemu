#include "Memory.h"

Memory::Memory()
{
    
}


Memory::~Memory()
{

}


uint8_t Memory::ReadByte(uint16_t index) const
{
    return memory[index];
}


uint8_t *Memory::GetBytePtr(uint16_t index)
{
    return &memory[index];
}


uint8_t Memory::operator[](uint16_t index) const
{
    return memory[index];
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
    }

    // If we get here, it wasn't handled by the switch.

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
