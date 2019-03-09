#include <sstream>

#include "gbemu.h"
#include "Cpu.h"
#include "Interrupt.h"
#include "Logger.h"
#include "MemoryByteProxy.h"
#include "RegisterByteProxy.h"
#include "Timer.h"

const char *Cpu::regNameMap8Bit[8] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
const char *Cpu::regNameMap16Bit[4] = {"BC", "DE", "HL", "SP"};
const char *Cpu::regNameMap16BitStack[4] = {"BC", "DE", "HL", "AF"};
const char *Cpu::flagNameMap[4] = {"NZ", "Z", "NC", "C"};


Cpu::Cpu(Interrupt *interrupts, Memory *memory, Timer *timer) :
    reg(),
    interrupts(interrupts),
    memory(memory),
    timer(timer),
    enableInterruptsDelay(false),
    halted(false),
    haltBug(false)
{
    regMap8Bit[0] = &reg.b;
    regMap8Bit[1] = &reg.c;
    regMap8Bit[2] = &reg.d;
    regMap8Bit[3] = &reg.e;
    regMap8Bit[4] = &reg.h;
    regMap8Bit[5] = &reg.l;
    regMap8Bit[6] = NULL;//&memory[reg.hl];
    regMap8Bit[7] = &reg.a;

    regMap16Bit[0] = &reg.bc;
    regMap16Bit[1] = &reg.de;
    regMap16Bit[2] = &reg.hl;
    regMap16Bit[3] = &reg.sp;

    regMap16BitStack[0] = &reg.bc;
    regMap16BitStack[1] = &reg.de;
    regMap16BitStack[2] = &reg.hl;
    regMap16BitStack[3] = &reg.af;
}


void Cpu::NotYetImplemented()
{
    // reg.pc is advanced in ReadPC8Bit, so subtract 1 to get the real address of the error.
    uint16_t addr = reg.pc - 1;

    std::stringstream ss;
    ss << "NYI opcode 0x" << std::hex << std::uppercase << (int)memory->ReadByte(addr) << " at 0x" << (int)addr;
    throw NotYetImplementedException(ss.str());
}


ByteProxy Cpu::GetByteProxy(uint8_t bits)
{
    uint8_t *ptr = regMap8Bit[bits & 0x07];

    if (ptr != NULL)
        return ByteProxy(new RegisterByteProxy(ptr));

    return ByteProxy(new MemoryByteProxy(reg.hl, memory));
}


inline uint8_t Cpu::ReadPC8Bit()
{
    uint8_t byte = memory->ReadByte(reg.pc);
    timer->AddCycle();

    if (!haltBug)
        reg.pc++;
    else
        haltBug = false;

    return byte;
}


inline uint16_t Cpu::ReadPC16Bit()
{
    uint8_t low = memory->ReadByte(reg.pc);
    reg.pc++;
    timer->AddCycle();

    uint8_t high = memory->ReadByte(reg.pc);
    reg.pc++;
    timer->AddCycle();

    uint16_t word = (high << 8) | low;

    return word;
}


inline uint8_t Cpu::HighByte(uint16_t word)
{
    return word >> 8;
}


inline uint8_t Cpu::LowByte(uint16_t word)
{
    return word & 0xFF;
}


inline uint8_t Cpu::GetFlagValue(uint8_t bits)
{
    uint8_t value;

    switch (bits & 0x03)
    {
        case 0x00:
            value = !reg.flags.z;
            break;
        case 0x01:
            value = reg.flags.z;
            break;
        case 0x02:
            value = !reg.flags.c;
            break;
        case 0x03:
            value = reg.flags.c;
            break;
    }

    return value;
}


inline uint8_t Cpu::Add8Bit(uint8_t x, uint8_t y, bool carryFlag/* = false*/)
{
    uint8_t carry = carryFlag ? 1 : 0;
    uint16_t result = x + y + carry;

    ClearFlags();
    if (((x & 0x0F) + (y & 0x0F) + carry) > 0x0F)
        reg.flags.h = 1;
    if (result > 0xFF)
        reg.flags.c = 1;
    reg.flags.z = !(result & 0xFF);

    return (result & 0xFF);
}


inline uint16_t Cpu::Add16Bit(uint16_t x, uint16_t y)
{
    uint32_t result = x + y;

    ClearFlags();
    if (((x & 0x0FFF) + (y & 0x0FFF)) > 0x0FFF)
        reg.flags.h = 1;
    if (result > 0xFFFF)
        reg.flags.c = 1;
    reg.flags.z = !(result & 0xFFFF);
    
    return (result & 0xFFFF);
}


inline uint16_t Cpu::Add16BitSigned8Bit(uint16_t x, int8_t y)
{
    uint32_t result = x + y;

    ClearFlags();
    // Half carry is on bit 4, not 12 like other 16 bit ops.
    if (((x & 0x000F) + ((uint8_t)y & 0x000F)) > 0x000F)
        reg.flags.h = 1;
    // Carry is on bit 8, not bit 16 like other 16 bit ops.
    if (((x & 0x00FF) + ((uint8_t)y & 0x00FF)) > 0x00FF)
        reg.flags.c = 1;
    reg.flags.z = !(result & 0xFFFF);

    return (result & 0xFFFF);
}


inline uint8_t Cpu::Sub8Bit(uint8_t x, uint8_t y, bool carryFlag/* = false*/)
{
    uint8_t carry = carryFlag ? 1 : 0;
    int16_t result = x - y - carry;

    ClearFlags();
    reg.flags.n = 1;
    if (((int)(x & 0x0F) - (int)(y & 0x0F) - carry) < 0)
        reg.flags.h = 1;
    if (result < 0)
        reg.flags.c = 1;
    reg.flags.z = !(result & 0xFF);

    return (result & 0xFF);
}


void Cpu::Push(uint16_t src)
{
    reg.sp--;
    memory->WriteByte(reg.sp, (uint8_t)(src >> 8));
    timer->AddCycle();

    reg.sp--;
    memory->WriteByte(reg.sp, (uint8_t)(src & 0xFF));
    timer->AddCycle();
}


void Cpu::Pop(uint16_t *dest)
{
    uint8_t low = memory->ReadByte(reg.sp);
    reg.sp++;
    timer->AddCycle();

    uint8_t high = memory->ReadByte(reg.sp);
    reg.sp++;
    timer->AddCycle();

    *dest = (high << 8) + low;
}


void Cpu::ProcessInterrupt(eInterruptTypes intType)
{
    // Add delay.
    timer->AddCycle();
    timer->AddCycle();
    timer->AddCycle();

    // Disable interrupts.
    interrupts->SetEnabled(false);

    // Push return address onto the stack.
    Push(reg.pc);

    // Clear current interrupt flag.
    interrupts->ClearInterrupt(intType);

    // Jump to ISR.
    reg.pc = interrupts->GetInterruptAddress(intType);

    LogInstruction("Interrupt %d, Jump to 0x%04X", intType, reg.pc);
}


void Cpu::ProcessOpCode()
{
    // Check for any waiting interrupts and process.
    eInterruptTypes intType;
    if (interrupts->CheckInterrupts(intType))
    {
        // Requested interrupt exits halt mode, even if it's not processed.
        if (halted)
        {
            halted = false;
            timer->AddCycle();
        }

        if (interrupts->Enabled())
        {
            ProcessInterrupt(intType);
            return;
        }
    }

    if (halted)
    {
        LogInstruction("Halted");
        timer->AddCycle();
        return;
    }

    // The EI intruction has a delayed effect, so enable interrupts after checking interrupts for this cycle.
    if (enableInterruptsDelay)
    {
        interrupts->SetEnabled(true);
        enableInterruptsDelay = false;
    }

    uint8_t opcode = ReadPC8Bit();

    switch (opcode)
    {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                   //
// 8-bit load opcodes                                                                                                //
//                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47: // LD B, Register
        case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F: // LD C, Register
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57: // LD D, Register
        case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5E: case 0x5F: // LD E, Register
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67: // LD H, Register
        case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6E: case 0x6F: // LD L, Register
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: /*case 0x76:*/ case 0x77: // LD (HL), Register
        case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E: case 0x7F: // LD A, Register
            {
                uint8_t destRegBits = (opcode >> 3) & 0x07;
                uint8_t srcRegBits = opcode & 0x07;
                ByteProxy dest = GetByteProxy(destRegBits);
                const ByteProxy src = GetByteProxy(srcRegBits);
                const char *destStr = regNameMap8Bit[destRegBits];
                const char *srcStr = regNameMap8Bit[srcRegBits];

                if (dest->ExtraCycles() || src->ExtraCycles())
                    timer->AddCycle();
                
                LogInstruction("%02X: LD %s, %s", opcode, destStr, srcStr);

                *dest = src->Value();
            }
            break;

        case 0x06: // LD B, immediate
        case 0x0E: // LD C, immediate
        case 0x16: // LD D, immediate
        case 0x1E: // LD E, immediate
        case 0x26: // LD H, immediate
        case 0x2E: // LD L, immediate
        case 0x36: // LD (HL), immediate
        case 0x3E: // LD A, immediate
            {
                uint8_t destRegBits = (opcode >> 3) & 0x07;
                ByteProxy dest = GetByteProxy(destRegBits);
                uint8_t x = ReadPC8Bit();
                const char *destStr = regNameMap8Bit[destRegBits];

                if (dest->ExtraCycles())
                    timer->AddCycle();
                
                LogInstruction("%02X %02X: LD %s, %02X", opcode, x, destStr, x);

                *dest = x;
            }
            break;

        case 0x0A: // LD A, (BC)
            {
                LogInstruction("%02X: LD A, (BC)", opcode);
                timer->AddCycle();
                reg.a = memory->ReadByte(reg.bc);
            }
            break;
        case 0x1A: // LD A, (DE)
            {
                LogInstruction("%02X: LD A, (DE)", opcode);
                timer->AddCycle();
                reg.a = memory->ReadByte(reg.de);
            }
            break;
        case 0xF2: // LD A, (0xFF00 + C)
            {
                LogInstruction("%02X: LD A, (0xFF00+C)", opcode);
                timer->AddCycle();
                reg.a = memory->ReadByte(0xFF00 + reg.c);
            }
            break;
        case 0xF0: // LD A, (0xFF00 + n)
            {
                uint8_t x = ReadPC8Bit();
                LogInstruction("%02X %02X: LD A, (0xFF00+0x%02X)", opcode, x, x);
                timer->AddCycle();
                reg.a = memory->ReadByte(0xFF00 + x);
            }
            break;
        case 0xFA: // LD A, (nn)
            {
                uint16_t x = ReadPC16Bit();
                LogInstruction("%02X %02X %02X: LD A, (%04X)", opcode, LowByte(x), HighByte(x), x);
                timer->AddCycle();
                reg.a = memory->ReadByte(x);
            }
            break;
        case 0x2A: // LD A, (HL+)
            {
                LogInstruction("%02X: LD A, (HL+)", opcode);
                timer->AddCycle();
                reg.a = memory->ReadByte(reg.hl);
                reg.hl++;
            }
            break;
        case 0x3A: // LD A, (HL-)
            {
                LogInstruction("%02X: LD A, (HL-)", opcode);
                timer->AddCycle();
                reg.a = memory->ReadByte(reg.hl);
                reg.hl--;
            }
            break;

        case 0xE0: // LD (0xFF00 + n), A
            {
                uint8_t x = ReadPC8Bit();
                LogInstruction("%02X %02X: LD (0xFF00+0x%02X), A", opcode, x, x);
                uint8_t val = reg.a;
                timer->AddCycle();
                memory->WriteByte(0xFF00 + x, val);
            }
            break;
        case 0xE2: // LD (0xFF00 + C), A
            {
                LogInstruction("%02X: LD (0xFF00+C), A", opcode);
                uint8_t val = reg.a;
                timer->AddCycle();
                memory->WriteByte(0xFF00 + reg.c, val);
            }
            break;
        case 0xEA: // LD (nn), A
            {
                uint16_t x = ReadPC16Bit();
                LogInstruction("%02X %02X %02X: LD (%04X), A", opcode, LowByte(x), HighByte(x), x);
                uint8_t val = reg.a;
                timer->AddCycle();
                memory->WriteByte(x, val);
            }
            break;
        case 0x02: // LD (BC), A
            {
                LogInstruction("%02X: LD (BC), A", opcode);
                uint8_t val = reg.a;
                timer->AddCycle();
                memory->WriteByte(reg.bc, val);
            }
            break;
        case 0x12: // LD (DE), A
            {
                LogInstruction("%02X: LD (DE), A", opcode);
                uint8_t val = reg.a;
                timer->AddCycle();
                memory->WriteByte(reg.de, val);
            }
            break;
        case 0x22: // LD (HL+), A
            {
                LogInstruction("%02X: LD (HL+), A", opcode);
                uint8_t val = reg.a;
                timer->AddCycle();
                memory->WriteByte(reg.hl, val);
                reg.hl++;
            }
            break;
        case 0x32: // LD (HL-), A
            {
                LogInstruction("%02X: LD (HL-), A", opcode);
                uint8_t val = reg.a;
                timer->AddCycle();
                memory->WriteByte(reg.hl, val);
                reg.hl--;
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                   //
// 16-bit load opcodes                                                                                               //
//                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x01: // LD BC, immediate
        case 0x11: // LD DE, immediate
        case 0x21: // LD HL, immediate
        case 0x31: // LD SP, immediate
            {
                uint8_t destRegBits = (opcode >> 4) & 0x03;
                uint16_t *dest = regMap16Bit[destRegBits];
                uint16_t x = ReadPC16Bit();
                const char *destStr = regNameMap16Bit[destRegBits];
                
                LogInstruction("%02X %02X %02X: LD %s, %04X", opcode, LowByte(x), HighByte(x), destStr, x);

                *dest = x;
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                   //
// Stack related opcdoes                                                                                             //
//                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0xC5: // PUSH BC
        case 0xD5: // PUSH DE
        case 0xE5: // PUSH HL
        case 0xF5: // PUSH AF
            {
                uint8_t regBits = (opcode >> 4) & 0x03;
                uint16_t *src = regMap16BitStack[regBits];
                const char *srcStr = regNameMap16BitStack[regBits];

                LogInstruction("%02X: PUSH %s", opcode, srcStr);

                timer->AddCycle();

                if (src == &reg.af)
                    // blargg's test roms say the low nybble of flags should always be zero.
                    Push(*src & 0xFFF0);
                else
                    Push(*src);
            }
            break;

        case 0xC1: // POP BC
        case 0xD1: // POP DE
        case 0xE1: // POP HL
        case 0xF1: // POP AF
            {
                uint8_t regBits = (opcode >> 4) & 0x03;
                uint16_t *dest = regMap16BitStack[regBits];
                const char *destStr = regNameMap16BitStack[regBits];

                LogInstruction("%02X: POP %s", opcode, destStr);

                Pop(dest);

                if (dest == &reg.af)
                    // blargg's test roms say the low nybble of flags should always be zero.
                    *dest = *dest & 0xFFF0;
            }
            break;

        case 0xF9: // LD SP, HL
            {
                LogInstruction("%02X: LD SP, HL", opcode);
                uint16_t val = reg.hl;
                timer->AddCycle();
                reg.sp = val;
            }
            break;
        case 0xF8: // LD HL, SP + e
            {
                int8_t x = ReadPC8Bit();
                LogInstruction("%02X %02X: LD HL, SP+0x%02X", opcode, x, x);

                reg.hl = Add16BitSigned8Bit(reg.sp, x);

                // Zero flag is always cleared.
                reg.flags.z = 0;

                timer->AddCycle();
            }
            break;
        case 0x08: // LD (nn), SP
            {
                uint16_t x = ReadPC16Bit();
                LogInstruction("%02X %02X %02X: LD (0x%04X), SP", opcode, HighByte(x), LowByte(x), x);

                memory->WriteByte(x, LowByte(reg.sp));
                timer->AddCycle();
                memory->WriteByte(x+1, HighByte(reg.sp));
                timer->AddCycle();
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                   //
// Arithmetic opcodes                                                                                                //
//                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x80: // ADD A, B
        case 0x81: // ADD A, C
        case 0x82: // ADD A, D
        case 0x83: // ADD A, E
        case 0x84: // ADD A, H
        case 0x85: // ADD A, L
        case 0x86: // ADD A, (HL)
        case 0x87: // ADD A, A
            {
                uint8_t regBits = opcode & 0x07;
                const ByteProxy src = GetByteProxy(regBits);
                const char *srcStr = regNameMap8Bit[regBits];

                if (src->ExtraCycles())
                    timer->AddCycle();

                LogInstruction("%02X: ADD A, %s", opcode, srcStr);

                reg.a = Add8Bit(reg.a, *src);
            }
            break;
        case 0xC6: // ADD A, n
            {
                uint8_t x = ReadPC8Bit();
                LogInstruction("%02X %02X: ADD A, 0x%02X", opcode, x, x);
                reg.a = Add8Bit(reg.a, x);
            }
            break;

        case 0x88: // ADC A, B
        case 0x89: // ADC A, C
        case 0x8A: // ADC A, D
        case 0x8B: // ADC A, E
        case 0x8C: // ADC A, H
        case 0x8D: // ADC A, L
        case 0x8E: // ADC A, (HL)
        case 0x8F: // ADC A, A
            {
                uint8_t regBits = opcode & 0x07;
                const ByteProxy src = GetByteProxy(regBits);
                const char *srcStr = regNameMap8Bit[regBits];

                if (src->ExtraCycles())
                    timer->AddCycle();

                LogInstruction("%02X: ADC A, %s, %d", opcode, srcStr, reg.flags.c);

                reg.a = Add8Bit(reg.a, *src, reg.flags.c);
            }
            break;
        case 0xCE: // ADC A, n
            {
                uint8_t x = ReadPC8Bit();
                LogInstruction("%02X %02X: ADC A, 0x%02X, %d", opcode, x, x, reg.flags.c);
                reg.a = Add8Bit(reg.a, x, reg.flags.c);
            }
            break;

        case 0x90: // SUB A, B
        case 0x91: // SUB A, C
        case 0x92: // SUB A, D
        case 0x93: // SUB A, E
        case 0x94: // SUB A, H
        case 0x95: // SUB A, L
        case 0x96: // SUB A, (HL)
        case 0x97: // SUB A, A
            {
                uint8_t regBits = opcode & 0x07;
                const ByteProxy src = GetByteProxy(regBits);
                const char *srcStr = regNameMap8Bit[regBits];

                if (src->ExtraCycles())
                    timer->AddCycle();

                LogInstruction("%02X: SUB A, %s", opcode, srcStr);

                reg.a = Sub8Bit(reg.a, *src);
            }
            break;
        case 0xD6: // SUB A, n
            {
                uint8_t x = ReadPC8Bit();
                LogInstruction("%02X %02X: SUB A, 0x%02X", opcode, x, x);
                reg.a = Sub8Bit(reg.a, x);
            }
            break;

        case 0x98: // SBC A, B
        case 0x99: // SBC A, C
        case 0x9A: // SBC A, D
        case 0x9B: // SBC A, E
        case 0x9C: // SBC A, H
        case 0x9D: // SBC A, L
        case 0x9E: // SBC A, (HL)
        case 0x9F: // SBC A, A
            {
                uint8_t regBits = opcode & 0x07;
                const ByteProxy src = GetByteProxy(regBits);
                const char *srcStr = regNameMap8Bit[regBits];

                if (src->ExtraCycles())
                    timer->AddCycle();

                LogInstruction("%02X: SBC A, %s, %d", opcode, srcStr, reg.flags.c);

                reg.a = Sub8Bit(reg.a, *src, reg.flags.c);
            }
            break;
        case 0xDE: // SBC A, n
            {
                uint8_t x = ReadPC8Bit();
                LogInstruction("%02X %02X: SBC A, 0x%02X, %d", opcode, x, x, reg.flags.c);
                reg.a = Sub8Bit(reg.a, x, reg.flags.c);
            }
            break;

        case 0x09: // ADD HL, BC
        case 0x19: // ADD HL, DE
        case 0x29: // ADD HL, HL
        case 0x39: // ADD HL, SP
            {
                uint8_t regBits = (opcode >> 4) & 0x03;
                uint16_t *src = regMap16Bit[regBits];
                const char *srcStr = regNameMap16Bit[regBits];
                timer->AddCycle();

                LogInstruction("%02X: ADD HL, %s", opcode, srcStr);

                // Zero flag not changed.
                uint8_t oldZero = reg.flags.z;
                reg.hl = Add16Bit(reg.hl, *src);
                reg.flags.z = oldZero;
            }
            break;

        case 0xE8: // ADD SP, e
            {
                int8_t x = ReadPC8Bit();
                LogInstruction("%02X %02X: ADD SP, %02X", opcode, x, x);

                // Extra delay for this intruction.
                timer->AddCycle();
                timer->AddCycle();

                reg.sp = Add16BitSigned8Bit(reg.sp, x);

                // Zero flag is always cleared.
                reg.flags.z = 0;
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                   //
// Boolean and bitwise opcodes                                                                                       //
//                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0xA0: // AND A, B
        case 0xA1: // AND A, C
        case 0xA2: // AND A, D
        case 0xA3: // AND A, E
        case 0xA4: // AND A, H
        case 0xA5: // AND A, L
        case 0xA6: // AND A, (HL)
        case 0xA7: // AND A, A
            {
                uint8_t regBits = opcode & 0x07;
                const ByteProxy src = GetByteProxy(regBits);
                const char *srcStr = regNameMap8Bit[regBits];

                if (src->ExtraCycles())
                    timer->AddCycle();

                LogInstruction("%02X: AND A, %s", opcode, srcStr);

                reg.a = reg.a & *src;

                ClearFlags();
                reg.flags.h = 1;
                reg.flags.z = !reg.a;
            }
            break;
        case 0xE6: // AND A, n
            {
                uint8_t x = ReadPC8Bit();

                LogInstruction("%02X %02X: AND A, 0x%02X", opcode, x, x);

                reg.a = reg.a & x;

                ClearFlags();
                reg.flags.h = 1;
                reg.flags.z = !reg.a;
            }
            break;

        case 0xA8: // XOR A, B
        case 0xA9: // XOR A, C
        case 0xAA: // XOR A, D
        case 0xAB: // XOR A, E
        case 0xAC: // XOR A, H
        case 0xAD: // XOR A, L
        case 0xAE: // XOR A, (HL)
        case 0xAF: // XOR A, A
            {
                uint8_t regBits = opcode & 0x07;
                const ByteProxy src = GetByteProxy(regBits);
                const char *srcStr = regNameMap8Bit[regBits];

                if (src->ExtraCycles())
                    timer->AddCycle();

                LogInstruction("%02X: XOR A, %s", opcode, srcStr);

                reg.a = reg.a ^ *src;

                ClearFlags();
                reg.flags.z = !reg.a;
            }
            break;
        case 0xEE: // XOR A, n
            {
                uint8_t x = ReadPC8Bit();

                LogInstruction("%02X %02X: XOR A, 0x%02X", opcode, x, x);

                reg.a = reg.a ^ x;

                ClearFlags();
                reg.flags.z = !reg.a;
            }
            break;

        case 0xB0: // OR A, B
        case 0xB1: // OR A, C
        case 0xB2: // OR A, D
        case 0xB3: // OR A, E
        case 0xB4: // OR A, H
        case 0xB5: // OR A, L
        case 0xB6: // OR A, (HL)
        case 0xB7: // OR A, A
            {
                uint8_t regBits = opcode & 0x07;
                const ByteProxy src = GetByteProxy(regBits);
                const char *srcStr = regNameMap8Bit[regBits];

                if (src->ExtraCycles())
                    timer->AddCycle();

                LogInstruction("%02X: OR A, %s", opcode, srcStr);

                reg.a = reg.a | *src;

                ClearFlags();
                reg.flags.z = !reg.a;
            }
            break;
        case 0xF6: // OR A, n
            {
                uint8_t x = ReadPC8Bit();

                LogInstruction("%02X %02X: OR A, 0x%02X", opcode, x, x);

                reg.a = reg.a | x;

                ClearFlags();
                reg.flags.z = !reg.a;
            }
            break;

        case 0xB8: // CP A, B
        case 0xB9: // CP A, C
        case 0xBA: // CP A, D
        case 0xBB: // CP A, E
        case 0xBC: // CP A, H
        case 0xBD: // CP A, L
        case 0xBE: // CP A, (HL)
        case 0xBF: // CP A, A
            {
                uint8_t regBits = opcode & 0x07;
                const ByteProxy src = GetByteProxy(regBits);
                const char *srcStr = regNameMap8Bit[regBits];

                if (src->ExtraCycles())
                    timer->AddCycle();

                LogInstruction("%02X: CP A, %s", opcode, srcStr);

                // Subtract and don't save result.
                Sub8Bit(reg.a, *src);
            }
            break;
        case 0xFE: // CP A, n
            {
                uint8_t x = ReadPC8Bit();
                LogInstruction("%02X %02X: CP A, 0x%02X", opcode, x, x);

                // Subtract and don't save result.
                Sub8Bit(reg.a, x);
            }
            break;

        case 0x2F: // CPL A
            {
                LogInstruction("%02X: CPL A", opcode);
                reg.a = ~reg.a;
                reg.flags.h = 1;
                reg.flags.n = 1;
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                   //
// Increment/Decrement opcodes                                                                                       //
//                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x04: // INC B
        case 0x0C: // INC C
        case 0x14: // INC D
        case 0x1C: // INC E
        case 0x24: // INC H
        case 0x2C: // INC L
        case 0x34: // INC (HL)
        case 0x3C: // INC A
            {
                uint8_t regBits = (opcode >> 3) & 0x07;
                const ByteProxy src = GetByteProxy(regBits);
                const char *srcStr = regNameMap8Bit[regBits];

                if (src->ExtraCycles())
                    timer->AddCycle();

                LogInstruction("%02X: INC %s", opcode, srcStr);

                uint8_t srcVal = *src;

                if (src->ExtraCycles())
                    timer->AddCycle();

                // Carry flag not changed.
                uint8_t oldCarry = reg.flags.c;
                *src = Add8Bit(srcVal, 1);
                reg.flags.c = oldCarry;
            }
            break;
            
        case 0x05: // DEC B
        case 0x0D: // DEC C
        case 0x15: // DEC D
        case 0x1D: // DEC E
        case 0x25: // DEC H
        case 0x2D: // DEC L
        case 0x35: // DEC (HL)
        case 0x3D: // DEC A
            {
                uint8_t regBits = (opcode >> 3) & 0x07;
                const ByteProxy src = GetByteProxy(regBits);
                const char *srcStr = regNameMap8Bit[regBits];

                if (src->ExtraCycles())
                    timer->AddCycle();

                LogInstruction("%02X: DEC %s", opcode, srcStr);

                uint8_t srcVal = *src;

                if (src->ExtraCycles())
                    timer->AddCycle();

                // Carry flag not changed.
                uint8_t oldCarry = reg.flags.c;
                *src = Sub8Bit(srcVal, 1);
                reg.flags.c = oldCarry;
            }
            break;

        case 0x03: // INC BC
        case 0x13: // INC DE
        case 0x23: // INC HL
        case 0x33: // INC SP
            {
                uint8_t regBits = (opcode >> 4) & 0x03;
                uint16_t *src = regMap16Bit[regBits];
                const char *srcStr = regNameMap16Bit[regBits];
                timer->AddCycle();

                LogInstruction("%02X: INC %s", opcode, srcStr);

                // Flags not changed.
                (*src)++;
            }
            break;

        case 0x0B: // DEC BC
        case 0x1B: // DEC DE
        case 0x2B: // DEC HL
        case 0x3B: // DEC SP
            {
                uint8_t regBits = (opcode >> 4) & 0x03;
                uint16_t *src = regMap16Bit[regBits];
                const char *srcStr = regNameMap16Bit[regBits];
                timer->AddCycle();

                LogInstruction("%02X: DEC %s", opcode, srcStr);

                // Flags not changed.
                (*src)--;
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                   //
// Rotate and shift opcodes                                                                                          //
//                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x07: // RLCA
            {
                LogInstruction("%02X: RLCA", opcode);
                ClearFlags();
                reg.flags.c = (reg.a & 0x80) ? 1 : 0;
                reg.a = (reg.a << 1) | reg.flags.c;
            }
            break;
        case 0x17: // RLA
            {
                LogInstruction("%02X: RLA", opcode);
                uint8_t oldCarry = reg.flags.c;
                ClearFlags();
                reg.flags.c = (reg.a & 0x80) ? 1 : 0;
                reg.a = (reg.a << 1) | oldCarry;
            }
            break;
        case 0x0F: // RRCA
            {
                LogInstruction("%02X: RRCA", opcode);
                ClearFlags();
                reg.flags.c = (reg.a & 0x01) ? 1 : 0;
                reg.a = (reg.a >> 1) | (reg.flags.c << 7);
            }
            break;
        case 0x1F: // RRA
            {
                LogInstruction("%02X: RRA", opcode);
                uint8_t oldCarry = reg.flags.c;
                ClearFlags();
                reg.flags.c = (reg.a & 0x01) ? 1 : 0;
                reg.a = (reg.a >> 1) | (oldCarry << 7);
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                   //
// 0xCB opcodes                                                                                                      //
//                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0xCB:
            {
                uint8_t subcode = ReadPC8Bit();

                uint8_t regBits = subcode & 0x07;
                ByteProxy src = GetByteProxy(regBits);
                const char *srcStr = regNameMap8Bit[regBits];

                switch (subcode)
                {
                    case 0x00: // RLC B
                    case 0x01: // RLC C
                    case 0x02: // RLC D
                    case 0x03: // RLC E
                    case 0x04: // RLC H
                    case 0x05: // RLC L
                    case 0x06: // RLC (HL)
                    case 0x07: // RLC A
                        {
                            LogInstruction("%02X %02X: RLC %s", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            ClearFlags();
                            reg.flags.c = (srcVal & 0x80) ? 1 : 0;
                            *src = (srcVal << 1) | reg.flags.c;
                            reg.flags.z = !(*src);
                        }
                        break;

                    case 0x08: // RRC B
                    case 0x09: // RRC C
                    case 0x0A: // RRC D
                    case 0x0B: // RRC E
                    case 0x0C: // RRC H
                    case 0x0D: // RRC L
                    case 0x0E: // RRC (HL)
                    case 0x0F: // RRC A
                        {
                            LogInstruction("%02X %02X: RRC %s", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            ClearFlags();
                            reg.flags.c = srcVal & 0x01;
                            *src = (srcVal >> 1) | (reg.flags.c << 7);
                            reg.flags.z = !(*src);
                        }
                        break;

                    case 0x10: // RL B
                    case 0x11: // RL C
                    case 0x12: // RL D
                    case 0x13: // RL E
                    case 0x14: // RL H
                    case 0x15: // RL L
                    case 0x16: // RL (HL)
                    case 0x17: // RL A
                        {
                            LogInstruction("%02X %02X: RL %s", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t oldCarry = reg.flags.c;
                            ClearFlags();
                            reg.flags.c = (srcVal & 0x80) ? 1 : 0;
                            *src = (srcVal << 1) | oldCarry;
                            reg.flags.z = !(*src);
                        }
                        break;

                    case 0x18: // RR B
                    case 0x19: // RR C
                    case 0x1A: // RR D
                    case 0x1B: // RR E
                    case 0x1C: // RR H
                    case 0x1D: // RR L
                    case 0x1E: // RR (HL)
                    case 0x1F: // RR A
                        {
                            LogInstruction("%02X %02X: RR %s", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t oldCarry = reg.flags.c;
                            ClearFlags();
                            reg.flags.c = srcVal & 0x01;
                            *src = (srcVal >> 1) | (oldCarry << 7);
                            reg.flags.z = !(*src);
                        }
                        break;

                    case 0x20: // SLA B
                    case 0x21: // SLA C
                    case 0x22: // SLA D
                    case 0x23: // SLA E
                    case 0x24: // SLA H
                    case 0x25: // SLA L
                    case 0x26: // SLA (HL)
                    case 0x27: // SLA A
                        {
                            LogInstruction("%02X %02X: SLA %s", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            ClearFlags();
                            reg.flags.c = (srcVal & 0x80) ? 1 : 0;
                            *src = srcVal << 1;
                            reg.flags.z = !(*src);
                        }
                        break;

                    case 0x28: // SRA B
                    case 0x29: // SRA C
                    case 0x2A: // SRA D
                    case 0x2B: // SRA E
                    case 0x2C: // SRA H
                    case 0x2D: // SRA L
                    case 0x2E: // SRA (HL)
                    case 0x2F: // SRA A
                        {
                            LogInstruction("%02X %02X: SRA %s", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            ClearFlags();
                            reg.flags.c = srcVal & 0x01;
                            *src = (srcVal >> 1) | (srcVal & 0x80);
                            reg.flags.z = !(*src);
                        }
                        break;

                    case 0x30: // SWAP B
                    case 0x31: // SWAP C
                    case 0x32: // SWAP D
                    case 0x33: // SWAP E
                    case 0x34: // SWAP H
                    case 0x35: // SWAP L
                    case 0x36: // SWAP (HL)
                    case 0x37: // SWAP A
                        {
                            LogInstruction("%02X %02X: SWAP %s", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            ClearFlags();
                            *src = (srcVal << 4) | (srcVal >> 4);
                            reg.flags.z = !(*src);
                        }
                        break;

                    case 0x38: // SRL B
                    case 0x39: // SRL C
                    case 0x3A: // SRL D
                    case 0x3B: // SRL E
                    case 0x3C: // SRL H
                    case 0x3D: // SRL L
                    case 0x3E: // SRL (HL)
                    case 0x3F: // SRL A
                        {
                            LogInstruction("%02X %02X: SRL %s", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            ClearFlags();
                            reg.flags.c = srcVal & 0x01;
                            *src = srcVal >> 1;
                            reg.flags.z = !(*src);
                        }
                        break;

                    case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47: // BIT 0, Register
                    case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F: // BIT 1, Register
                    case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57: // BIT 2, Register
                    case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5E: case 0x5F: // BIT 3, Register
                    case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67: // BIT 4, Register
                    case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6E: case 0x6F: // BIT 5, Register
                    case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77: // BIT 6, Register
                    case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E: case 0x7F: // BIT 7, Register
                        {
                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t bit = (subcode >> 3) & 0x07;
                            
                            LogInstruction("%02X %02X: BIT %d, %s", opcode, subcode, bit, srcStr);

                            // Carry bit not changed.
                            reg.flags.z = !(*src & (1 << bit));
                            reg.flags.n = 0;
                            reg.flags.h = 1;
                        }
                        break;

                    case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87: // RES 0, Register
                    case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8E: case 0x8F: // RES 1, Register
                    case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97: // RES 2, Register
                    case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9E: case 0x9F: // RES 3, Register
                    case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA6: case 0xA7: // RES 4, Register
                    case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAE: case 0xAF: // RES 5, Register
                    case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7: // RES 6, Register
                    case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE: case 0xBF: // RES 7, Register
                        {
                            uint8_t bit = (subcode >> 3) & 0x07;
                            
                            LogInstruction("%02X %02X: RES %d, %s", opcode, subcode, bit, srcStr);

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            *src = srcVal & ~(1 << bit);
                        }
                        break;

                    case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC6: case 0xC7: // SET 0, Register
                    case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE: case 0xCF: // SET 1, Register
                    case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD6: case 0xD7: // SET 2, Register
                    case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDE: case 0xDF: // SET 3, Register
                    case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE6: case 0xE7: // SET 4, Register
                    case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEE: case 0xEF: // SET 5, Register
                    case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF6: case 0xF7: // SET 6, Register
                    case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFE: case 0xFF: // SET 7, Register
                        {
                            uint8_t bit = (subcode >> 3) & 0x07;
                            
                            LogInstruction("%02X %02X: SET %d, %s", opcode, subcode, bit, srcStr);

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                timer->AddCycle();

                            *src = srcVal | (1 << bit);
                        }
                        break;
                }
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                   //
// Jump, Call, and Return opcodes                                                                                    //
//                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0xC3: // JP nn
            {
                uint16_t addr = ReadPC16Bit();
                LogInstruction("%02X %02X %02X: JP 0x%04X", opcode, LowByte(addr), HighByte(addr), addr);
                timer->AddCycle();
                reg.pc = addr;
            }
            break;
        case 0xE9: // JP (HL)
            {
                LogInstruction("%02X: JP (HL)", opcode);
                reg.pc = reg.hl;
            }
            break;
        case 0xC2: // JP NZ, nn
        case 0xCA: // JP Z, nn
        case 0xD2: // JP NC, nn
        case 0xDA: // JP C, nn
            {
                uint16_t addr = ReadPC16Bit();
                uint8_t flagBits = (opcode >> 3) & 0x03;
                LogInstruction("%02X %02X %02X: JP %s, 0x%04X", opcode, LowByte(addr), HighByte(addr), flagNameMap[flagBits], addr);
                if (GetFlagValue(flagBits))
                {
                    reg.pc = addr;
                    timer->AddCycle();
                }
            }
            break;

        case 0x18: // JR e
            {
                int8_t offset = ReadPC8Bit();
                LogInstruction("%02X %02X: JR %d", opcode, offset, offset);
                if (offset == -2)
                {
                    throw InfiniteLoopException();
                }
                reg.pc += offset;
                timer->AddCycle();
            }
            break;
        case 0x20: // JR NZ, e
        case 0x28: // JR Z, e
        case 0x30: // JR NC, e
        case 0x38: // JR C, e
            {
                int8_t offset = ReadPC8Bit();
                uint8_t flagBits = (opcode >> 3) & 0x03;
                LogInstruction("%02X %02X: JR %s, %d", opcode, (uint8_t)offset, flagNameMap[flagBits], offset);
                if (GetFlagValue(flagBits))
                {
                    if (offset == -2)
                    {
                        throw InfiniteLoopException();
                    }
                    reg.pc += offset;
                    timer->AddCycle();
                }
            }
            break;

        case 0xCD: // CALL nn
            {
                uint16_t addr = ReadPC16Bit();
                LogInstruction("%02X %02X %02X: CALL %04X", opcode, LowByte(addr), HighByte(addr), addr);
                timer->AddCycle();
                Push(reg.pc);
                reg.pc = addr;
            }
            break;
        case 0xC4: // CALL NZ, nn
        case 0xCC: // CALL Z, nn
        case 0xD4: // CALL NC, nn
        case 0xDC: // CALL C, nn
            {
                uint16_t addr = ReadPC16Bit();
                uint8_t flagBits = (opcode >> 3) & 0x03;
                LogInstruction("%02X %02X %02X: CALL %s, %04X", opcode, LowByte(addr), HighByte(addr), flagNameMap[flagBits], addr);
                if (GetFlagValue(flagBits))
                {
                    timer->AddCycle();
                    Push(reg.pc);
                    reg.pc = addr;
                }
            }
            break;

        case 0xC9: // RET
            {
                LogInstruction("%02X: RET", opcode);
                timer->AddCycle();
                Pop(&reg.pc);
            }
            break;
        case 0xD9: // RETI
            {
                LogInstruction("%02X: RETI", opcode);
                timer->AddCycle();
                Pop(&reg.pc);
                interrupts->SetEnabled(true);
            }
            break;
        case 0xC0: // RET NZ
        case 0xC8: // RET Z
        case 0xD0: // RET NC
        case 0xD8: // RET C
            {
                timer->AddCycle();
                uint8_t flagBits = (opcode >> 3) & 0x03;
                LogInstruction("%02X: RET %s", opcode, flagNameMap[flagBits]);
                if (GetFlagValue(flagBits))
                {
                    Pop(&reg.pc);
                    timer->AddCycle();
                }
            }
            break;

        case 0xC7: // RST 0x00
        case 0xCF: // RST 0x08
        case 0xD7: // RST 0x10
        case 0xDF: // RST 0x18
        case 0xE7: // RST 0x20
        case 0xEF: // RST 0x28
        case 0xF7: // RST 0x30
        case 0xFF: // RST 0x38
            {
                uint8_t addr = ((opcode >> 3) & 0x07) * 8;
                LogInstruction("%02X: RST 0x%02X", opcode, addr);
                timer->AddCycle();
                Push(reg.pc);
                reg.pc = (uint16_t)addr;
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                   //
// Misc opcodes                                                                                                      //
//                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x00: // NOP
            {
                LogInstruction("%02X: NOP", opcode);
            }
            break;

        case 0x76: // HALT
            {
                LogInstruction("%02X: HALT", opcode);
                eInterruptTypes intType;
                if (!interrupts->Enabled() && interrupts->CheckInterrupts(intType))
                    haltBug = true;
                else
                    halted = true;
            }
            break;

        case 0x37: // SCF
            {
                LogInstruction("%02X: SCF", opcode);
                reg.flags.c = 1;
            }
            break;

        case 0x3F: // CCF
            {
                LogInstruction("%02X: CCF", opcode);
                reg.flags.c = !reg.flags.c;
            }
            break;

        case 0x27: // DAA
            {
                LogInstruction("%02X: DAA", opcode);

                if (!reg.flags.n)
                {
                    if (reg.flags.c || reg.a > 0x99)
                    {
                        reg.a += 0x60;
                        reg.flags.c = 1;
                    }
                    if (reg.flags.h || (reg.a & 0x0F) > 0x09)
                    {
                        reg.a += 0x06;
                        reg.flags.h = 0;
                    }
                }
                else
                {
                    if (reg.flags.c && reg.flags.h)
                    {
                        reg.a += 0x9A;
                        reg.flags.h = 0;
                    }
                    else if (reg.flags.c)
                    {
                        reg.a += 0xA0;
                    }
                    else if (reg.flags.h)
                    {
                        reg.a += 0xFA;
                        reg.flags.h = 0;
                    }
                }

                reg.flags.z = !((bool)reg.a);
            }
            break;

        case 0xF3: // DI
            {
                LogInstruction("%02X: DI", opcode);
                interrupts->SetEnabled(false);
            }
            break;

        case 0xFB: // EI
            {
                LogInstruction("%02X: EI", opcode);

                // Interrupts aren't immediately enabled, we have to process one opcode before enabling interrupts.
                enableInterruptsDelay = true;
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                   //
// Unimplemented opcodes                                                                                             //
//                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x10: NotYetImplemented(); break; // STOP

        case 0xD3: NotYetImplemented(); break;
        case 0xDB: NotYetImplemented(); break;
        case 0xDD: NotYetImplemented(); break;

        case 0xE3: NotYetImplemented(); break;
        case 0xE4: NotYetImplemented(); break;
        case 0xEB: NotYetImplemented(); break;
        case 0xEC: NotYetImplemented(); break;
        case 0xED: NotYetImplemented(); break;

        case 0xF4: NotYetImplemented(); break;
        case 0xFC: NotYetImplemented(); break;
        case 0xFD: NotYetImplemented(); break;

        default: NotYetImplemented(); break;
    }

    return;
}


bool Cpu::SaveState(FILE *file)
{
    size_t cnt;

    cnt = fwrite(&reg, sizeof(reg), 1, file);
    if (cnt == 0)
        return false;

    cnt = fwrite(&enableInterruptsDelay, sizeof(enableInterruptsDelay), 1, file);
    if (cnt == 0)
        return false;

    cnt = fwrite(&halted, sizeof(halted), 1, file);
    if (cnt == 0)
        return false;

    cnt = fwrite(&haltBug, sizeof(haltBug), 1, file);
    if (cnt == 0)
        return false;

    return true;
}


bool Cpu::LoadState(FILE *file)
{
    size_t cnt;

    cnt = fread(&reg, sizeof(reg), 1, file);
    if (cnt == 0)
        return false;

    cnt = fread(&enableInterruptsDelay, sizeof(enableInterruptsDelay), 1, file);
    if (cnt == 0)
        return false;

    cnt = fread(&halted, sizeof(halted), 1, file);
    if (cnt == 0)
        return false;

    cnt = fread(&haltBug, sizeof(haltBug), 1, file);
    if (cnt == 0)
        return false;

    return true;
}