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
}


void Memory::ClearMemory()
{
    memory.fill(0);
}