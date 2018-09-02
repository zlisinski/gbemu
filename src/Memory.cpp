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
    memory[index] = byte;

    if (index == MEM_SC && byte & 0x80)
    {
        FILE *file = fopen("serial.txt", "a");
        fputc(memory[MEM_SB], file);
        fclose(file);
        DBG("Serial: %02X, '%c'\n", memory[MEM_SB], memory[MEM_SB]);
    }
}


void Memory::ClearMemory()
{
    memory.fill(0);
}