#include <sstream>

#include "gbemu.h"
#include "Emulator.h"
#include "RegisterByteProxy.h"
#include "MemoryByteProxy.h"

const char *Emulator::regNameMap8Bit[8] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
const char *Emulator::regNameMap16Bit[4] = {"BC", "DE", "HL", "SP"};
const char *Emulator::regNameMap16BitStack[4] = {"BC", "DE", "HL", "AF"};
const char *Emulator::flagNameMap[4] = {"NZ", "Z", "NC", "C"};


Emulator::Emulator(State *state) :
    state(state),
    enableInterruptsDelay(false),
    haltBug(false)
{
    regMap8Bit[0] = &state->b;
    regMap8Bit[1] = &state->c;
    regMap8Bit[2] = &state->d;
    regMap8Bit[3] = &state->e;
    regMap8Bit[4] = &state->h;
    regMap8Bit[5] = &state->l;
    regMap8Bit[6] = NULL;//&state->memory[state->hl];
    regMap8Bit[7] = &state->a;

    regMap16Bit[0] = &state->bc;
    regMap16Bit[1] = &state->de;
    regMap16Bit[2] = &state->hl;
    regMap16Bit[3] = &state->sp;

    regMap16BitStack[0] = &state->bc;
    regMap16BitStack[1] = &state->de;
    regMap16BitStack[2] = &state->hl;
    regMap16BitStack[3] = &state->af;
}


void Emulator::NotYetImplemented()
{
    // state->pc is advanced in ReadPC8Bit, so subtract 1 to get the real address of the error.
    uint16_t addr = state->pc - 1;

    std::stringstream ss;
    ss << "NYI opcode 0x" << std::hex << std::uppercase << (int)state->memory->ReadByte(addr) << " at 0x" << (int)addr;
    throw NotYetImplementedException(ss.str());
}


ByteProxy Emulator::GetByteProxy(uint8_t bits)
{
    uint8_t *ptr = regMap8Bit[bits & 0x07];

    if (ptr != NULL)
        return ByteProxy(new RegisterByteProxy(ptr));

    return ByteProxy(new MemoryByteProxy(state->hl, state->memory));
}


inline uint8_t Emulator::ReadPC8Bit()
{
    uint8_t byte = state->memory->ReadByte(state->pc);
    state->timer->AddCycle();

    if (!haltBug)
        state->pc++;
    else
        haltBug = false;

    return byte;
}


inline uint16_t Emulator::ReadPC16Bit()
{
    uint8_t low = state->memory->ReadByte(state->pc);
    state->pc++;
    state->timer->AddCycle();

    uint8_t high = state->memory->ReadByte(state->pc);
    state->pc++;
    state->timer->AddCycle();

    uint16_t word = (high << 8) | low;

    return word;
}


inline uint8_t Emulator::HighByte(uint16_t word)
{
    return word >> 8;
}


inline uint8_t Emulator::LowByte(uint16_t word)
{
    return word & 0xFF;
}


inline uint8_t Emulator::GetFlagValue(uint8_t bits)
{
    uint8_t value;

    switch (bits & 0x03)
    {
        case 0x00:
            value = !state->flags.z;
            break;
        case 0x01:
            value = state->flags.z;
            break;
        case 0x02:
            value = !state->flags.c;
            break;
        case 0x03:
            value = state->flags.c;
            break;
    }

    return value;
}


inline uint8_t Emulator::Add8Bit(uint8_t x, uint8_t y, bool carryFlag/* = false*/)
{
    uint8_t carry = carryFlag ? 1 : 0;
    uint16_t result = x + y + carry;

    state->ClearFlags();
    if (((x & 0x0F) + (y & 0x0F) + carry) > 0x0F)
        state->flags.h = 1;
    if (result > 0xFF)
        state->flags.c = 1;
    state->flags.z = !(result & 0xFF);

    return (result & 0xFF);
}


inline uint16_t Emulator::Add16Bit(uint16_t x, uint16_t y)
{
    uint32_t result = x + y;

    state->ClearFlags();
    if (((x & 0x0FFF) + (y & 0x0FFF)) > 0x0FFF)
        state->flags.h = 1;
    if (result > 0xFFFF)
        state->flags.c = 1;
    state->flags.z = !(result & 0xFFFF);
    
    return (result & 0xFFFF);
}


inline uint16_t Emulator::Add16BitSigned8Bit(uint16_t x, int8_t y)
{
    uint32_t result = x + y;

    state->ClearFlags();
    // Half carry is on bit 4, not 12 like other 16 bit ops.
    if (((x & 0x000F) + ((uint8_t)y & 0x000F)) > 0x000F)
        state->flags.h = 1;
    // Carry is on bit 8, not bit 16 like other 16 bit ops.
    if (((x & 0x00FF) + ((uint8_t)y & 0x00FF)) > 0x00FF)
        state->flags.c = 1;
    state->flags.z = !(result & 0xFFFF);

    return (result & 0xFFFF);
}


inline uint8_t Emulator::Sub8Bit(uint8_t x, uint8_t y, bool carryFlag/* = false*/)
{
    uint8_t carry = carryFlag ? 1 : 0;
    int16_t result = x - y - carry;

    state->ClearFlags();
    state->flags.n = 1;
    if (((int)(x & 0x0F) - (int)(y & 0x0F) - carry) < 0)
        state->flags.h = 1;
    if (result < 0)
        state->flags.c = 1;
    state->flags.z = !(result & 0xFF);

    return (result & 0xFF);
}


void Emulator::Push(uint16_t src)
{
    state->sp--;
    state->memory->WriteByte(state->sp, (uint8_t)(src >> 8));
    state->timer->AddCycle();

    state->sp--;
    state->memory->WriteByte(state->sp, (uint8_t)(src & 0xFF));
    state->timer->AddCycle();
}


void Emulator::Pop(uint16_t *dest)
{
    uint8_t low = state->memory->ReadByte(state->sp);
    state->sp++;
    state->timer->AddCycle();

    uint8_t high = state->memory->ReadByte(state->sp);
    state->sp++;
    state->timer->AddCycle();

    *dest = (high << 8) + low;
}


void Emulator::ProcessInterrupt(eInterruptTypes intType)
{
    // Add delay.
    state->timer->AddCycle();
    state->timer->AddCycle();
    state->timer->AddCycle();

    // Disable interrupts.
    state->interrupts->SetEnabled(false);

    // Push return address onto the stack.
    Push(state->pc);

    // Clear current interrupt flag.
    state->interrupts->ClearInterrupt(intType);

    // Jump to ISR.
    state->pc = state->interrupts->GetInterruptAddress(intType);

    DBG("Interrupt %d, Jump to 0x%04X\n\n", intType, state->pc);
}


void Emulator::ProcessOpCode()
{
    // Check for any waiting interrupts and process.
    eInterruptTypes intType;
    if (state->interrupts->CheckInterrupts(intType))
    {
        // Requested interrupt exits halt mode, even if it's not processed.
        if (state->halted)
        {
            state->halted = false;
            state->timer->AddCycle();
        }

        if (state->interrupts->Enabled())
        {
            ProcessInterrupt(intType);
            return;
        }
    }

    if (state->halted)
    {
        DBG("Halted\n");
        state->timer->AddCycle();
        return;
    }

    // The EI intruction has a delayed effect, so enable interrupts after checking interrupts for this cycle.
    if (enableInterruptsDelay)
    {
        state->interrupts->SetEnabled(true);
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
                    state->timer->AddCycle();
                
                DBG("%02X: LD %s, %s\n", opcode, destStr, srcStr);

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
                    state->timer->AddCycle();
                
                DBG("%02X %02X: LD %s, %02X\n", opcode, x, destStr, x);

                *dest = x;
            }
            break;

        case 0x0A: // LD A, (BC)
            {
                DBG("%02X: LD A, (BC)\n", opcode);
                state->timer->AddCycle();
                state->a = state->memory->ReadByte(state->bc);
            }
            break;
        case 0x1A: // LD A, (DE)
            {
                DBG("%02X: LD A, (DE)\n", opcode);
                state->timer->AddCycle();
                state->a = state->memory->ReadByte(state->de);
            }
            break;
        case 0xF2: // LD A, (0xFF00 + C)
            {
                DBG("%02X: LD A, (0xFF00+C)\n", opcode);
                state->timer->AddCycle();
                state->a = state->memory->ReadByte(0xFF00 + state->c);
            }
            break;
        case 0xF0: // LD A, (0xFF00 + n)
            {
                uint8_t x = ReadPC8Bit();
                DBG("%02X %02X: LD A, (0xFF00+0x%02X)\n", opcode, x, x);
                state->timer->AddCycle();
                state->a = state->memory->ReadByte(0xFF00 + x);
            }
            break;
        case 0xFA: // LD A, (nn)
            {
                uint16_t x = ReadPC16Bit();
                DBG("%02X %02X %02X: LD A, (%04X)\n", opcode, LowByte(x), HighByte(x), x);
                state->timer->AddCycle();
                state->a = state->memory->ReadByte(x);
            }
            break;
        case 0x2A: // LD A, (HL+)
            {
                DBG("%02X: LD A, (HL+)\n", opcode);
                state->timer->AddCycle();
                state->a = state->memory->ReadByte(state->hl);
                state->hl++;
            }
            break;
        case 0x3A: // LD A, (HL-)
            {
                DBG("%02X: LD A, (HL-)\n", opcode);
                state->timer->AddCycle();
                state->a = state->memory->ReadByte(state->hl);
                state->hl--;
            }
            break;

        case 0xE0: // LD (0xFF00 + n), A
            {
                uint8_t x = ReadPC8Bit();
                DBG("%02X %02X: LD (0xFF00+0x%02X), A\n", opcode, x, x);
                uint8_t val = state->a;
                state->timer->AddCycle();
                state->memory->WriteByte(0xFF00 + x, val);
            }
            break;
        case 0xE2: // LD (0xFF00 + C), A
            {
                DBG("%02X: LD (0xFF00+C), A\n", opcode);
                uint8_t val = state->a;
                state->timer->AddCycle();
                state->memory->WriteByte(0xFF00 + state->c, val);
            }
            break;
        case 0xEA: // LD (nn), A
            {
                uint16_t x = ReadPC16Bit();
                DBG("%02X %02X %02X: LD (%04X), A\n", opcode, LowByte(x), HighByte(x), x);
                uint8_t val = state->a;
                state->timer->AddCycle();
                state->memory->WriteByte(x, val);
            }
            break;
        case 0x02: // LD (BC), A
            {
                DBG("%02X: LD (BC), A\n", opcode);
                uint8_t val = state->a;
                state->timer->AddCycle();
                state->memory->WriteByte(state->bc, val);
            }
            break;
        case 0x12: // LD (DE), A
            {
                DBG("%02X: LD (DE), A\n", opcode);
                uint8_t val = state->a;
                state->timer->AddCycle();
                state->memory->WriteByte(state->de, val);
            }
            break;
        case 0x22: // LD (HL+), A
            {
                DBG("%02X: LD (HL+), A\n", opcode);
                uint8_t val = state->a;
                state->timer->AddCycle();
                state->memory->WriteByte(state->hl, val);
                state->hl++;
            }
            break;
        case 0x32: // LD (HL-), A
            {
                DBG("%02X: LD (HL-), A\n", opcode);
                uint8_t val = state->a;
                state->timer->AddCycle();
                state->memory->WriteByte(state->hl, val);
                state->hl--;
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
                
                DBG("%02X %02X %02X: LD %s, %04X\n", opcode, LowByte(x), HighByte(x), destStr, x);

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

                DBG("%02X: PUSH %s\n", opcode, srcStr);

                state->timer->AddCycle();

                if (src == &state->af)
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

                DBG("%02X: POP %s\n", opcode, destStr);

                Pop(dest);

                if (dest == &state->af)
                    // blargg's test roms say the low nybble of flags should always be zero.
                    *dest = *dest & 0xFFF0;
            }
            break;

        case 0xF9: // LD SP, HL
            {
                DBG("%02X: LD SP, HL\n", opcode);
                uint16_t val = state->hl;
                state->timer->AddCycle();
                state->sp = val;
            }
            break;
        case 0xF8: // LD HL, SP + e
            {
                int8_t x = ReadPC8Bit();
                DBG("%02X %02X: LD HL, SP+0x%02X\n", opcode, x, x);

                state->hl = Add16BitSigned8Bit(state->sp, x);

                // Zero flag is always cleared.
                state->flags.z = 0;

                state->timer->AddCycle();
            }
            break;
        case 0x08: // LD (nn), SP
            {
                uint16_t x = ReadPC16Bit();
                DBG("%02X %02X %02X: LD (0x%04X), SP\n", opcode, HighByte(x), LowByte(x), x);

                state->memory->WriteByte(x, LowByte(state->sp));
                state->timer->AddCycle();
                state->memory->WriteByte(x+1, HighByte(state->sp));
                state->timer->AddCycle();
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
                    state->timer->AddCycle();

                DBG("%02X: ADD A, %s\n", opcode, srcStr);

                state->a = Add8Bit(state->a, *src);
            }
            break;
        case 0xC6: // ADD A, n
            {
                uint8_t x = ReadPC8Bit();
                DBG("%02X %02X: ADD A, 0x%02X\n", opcode, x, x);
                state->a = Add8Bit(state->a, x);
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
                    state->timer->AddCycle();

                DBG("%02X: ADC A, %s, %d\n", opcode, srcStr, state->flags.c);

                state->a = Add8Bit(state->a, *src, state->flags.c);
            }
            break;
        case 0xCE: // ADC A, n
            {
                uint8_t x = ReadPC8Bit();
                DBG("%02X %02X: ADC A, 0x%02X, %d\n", opcode, x, x, state->flags.c);
                state->a = Add8Bit(state->a, x, state->flags.c);
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
                    state->timer->AddCycle();

                DBG("%02X: SUB A, %s\n", opcode, srcStr);

                state->a = Sub8Bit(state->a, *src);
            }
            break;
        case 0xD6: // SUB A, n
            {
                uint8_t x = ReadPC8Bit();
                DBG("%02X %02X: SUB A, 0x%02X\n", opcode, x, x);
                state->a = Sub8Bit(state->a, x);
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
                    state->timer->AddCycle();

                DBG("%02X: SBC A, %s, %d\n", opcode, srcStr, state->flags.c);

                state->a = Sub8Bit(state->a, *src, state->flags.c);
            }
            break;
        case 0xDE: // SBC A, n
            {
                uint8_t x = ReadPC8Bit();
                DBG("%02X %02X: SBC A, 0x%02X, %d\n", opcode, x, x, state->flags.c);
                state->a = Sub8Bit(state->a, x, state->flags.c);
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
                state->timer->AddCycle();

                DBG("%02X: ADD HL, %s\n", opcode, srcStr);

                // Zero flag not changed.
                uint8_t oldZero = state->flags.z;
                state->hl = Add16Bit(state->hl, *src);
                state->flags.z = oldZero;
            }
            break;

        case 0xE8: // ADD SP, e
            {
                int8_t x = ReadPC8Bit();
                DBG("%02X %02X: ADD SP, %02X\n", opcode, x, x);

                // Extra delay for this intruction.
                state->timer->AddCycle();
                state->timer->AddCycle();

                state->sp = Add16BitSigned8Bit(state->sp, x);

                // Zero flag is always cleared.
                state->flags.z = 0;
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
                    state->timer->AddCycle();

                DBG("%02X: AND A, %s\n", opcode, srcStr);

                state->a = state->a & *src;

                state->ClearFlags();
                state->flags.h = 1;
                state->flags.z = !state->a;
            }
            break;
        case 0xE6: // AND A, n
            {
                uint8_t x = ReadPC8Bit();

                DBG("%02X %02X: AND A, 0x%02X\n", opcode, x, x);

                state->a = state->a & x;

                state->ClearFlags();
                state->flags.h = 1;
                state->flags.z = !state->a;
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
                    state->timer->AddCycle();

                DBG("%02X: XOR A, %s\n", opcode, srcStr);

                state->a = state->a ^ *src;

                state->ClearFlags();
                state->flags.z = !state->a;
            }
            break;
        case 0xEE: // XOR A, n
            {
                uint8_t x = ReadPC8Bit();

                DBG("%02X %02X: XOR A, 0x%02X\n", opcode, x, x);

                state->a = state->a ^ x;

                state->ClearFlags();
                state->flags.z = !state->a;
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
                    state->timer->AddCycle();

                DBG("%02X: OR A, %s\n", opcode, srcStr);

                state->a = state->a | *src;

                state->ClearFlags();
                state->flags.z = !state->a;
            }
            break;
        case 0xF6: // OR A, n
            {
                uint8_t x = ReadPC8Bit();

                DBG("%02X %02X: OR A, 0x%02X\n", opcode, x, x);

                state->a = state->a | x;

                state->ClearFlags();
                state->flags.z = !state->a;
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
                    state->timer->AddCycle();

                DBG("%02X: CP A, %s\n", opcode, srcStr);

                // Subtract and don't save result.
                Sub8Bit(state->a, *src);
            }
            break;
        case 0xFE: // CP A, n
            {
                uint8_t x = ReadPC8Bit();
                DBG("%02X %02X: CP A, 0x%02X\n", opcode, x, x);

                // Subtract and don't save result.
                Sub8Bit(state->a, x);
            }
            break;

        case 0x2F: // CPL A
            {
                DBG("%02X: CPL A\n", opcode);
                state->a = ~state->a;
                state->flags.h = 1;
                state->flags.n = 1;
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
                    state->timer->AddCycle();

                DBG("%02X: INC %s\n", opcode, srcStr);

                uint8_t srcVal = *src;

                if (src->ExtraCycles())
                    state->timer->AddCycle();

                // Carry flag not changed.
                uint8_t oldCarry = state->flags.c;
                *src = Add8Bit(srcVal, 1);
                state->flags.c = oldCarry;
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
                    state->timer->AddCycle();

                DBG("%02X: DEC %s\n", opcode, srcStr);

                uint8_t srcVal = *src;

                if (src->ExtraCycles())
                    state->timer->AddCycle();

                // Carry flag not changed.
                uint8_t oldCarry = state->flags.c;
                *src = Sub8Bit(srcVal, 1);
                state->flags.c = oldCarry;
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
                state->timer->AddCycle();

                DBG("%02X: INC %s\n", opcode, srcStr);

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
                state->timer->AddCycle();

                DBG("%02X: DEC %s\n", opcode, srcStr);

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
                DBG("%02X: RLCA\n", opcode);
                state->ClearFlags();
                state->flags.c = (state->a & 0x80) ? 1 : 0;
                state->a = (state->a << 1) | state->flags.c;
            }
            break;
        case 0x17: // RLA
            {
                DBG("%02X: RLA\n", opcode);
                uint8_t oldCarry = state->flags.c;
                state->ClearFlags();
                state->flags.c = (state->a & 0x80) ? 1 : 0;
                state->a = (state->a << 1) | oldCarry;
            }
            break;
        case 0x0F: // RRCA
            {
                DBG("%02X: RRCA\n", opcode);
                state->ClearFlags();
                state->flags.c = (state->a & 0x01) ? 1 : 0;
                state->a = (state->a >> 1) | (state->flags.c << 7);
            }
            break;
        case 0x1F: // RRA
            {
                DBG("%02X: RRA\n", opcode);
                uint8_t oldCarry = state->flags.c;
                state->ClearFlags();
                state->flags.c = (state->a & 0x01) ? 1 : 0;
                state->a = (state->a >> 1) | (oldCarry << 7);
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
                            DBG("%02X %02X: RLC %s\n", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            state->ClearFlags();
                            state->flags.c = (srcVal & 0x80) ? 1 : 0;
                            *src = (srcVal << 1) | state->flags.c;
                            state->flags.z = !(*src);
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
                            DBG("%02X %02X: RRC %s\n", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            state->ClearFlags();
                            state->flags.c = srcVal & 0x01;
                            *src = (srcVal >> 1) | (state->flags.c << 7);
                            state->flags.z = !(*src);
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
                            DBG("%02X %02X: RL %s\n", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            uint8_t oldCarry = state->flags.c;
                            state->ClearFlags();
                            state->flags.c = (srcVal & 0x80) ? 1 : 0;
                            *src = (srcVal << 1) | oldCarry;
                            state->flags.z = !(*src);
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
                            DBG("%02X %02X: RR %s\n", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            uint8_t oldCarry = state->flags.c;
                            state->ClearFlags();
                            state->flags.c = srcVal & 0x01;
                            *src = (srcVal >> 1) | (oldCarry << 7);
                            state->flags.z = !(*src);
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
                            DBG("%02X %02X: SLA %s\n", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            state->ClearFlags();
                            state->flags.c = (srcVal & 0x80) ? 1 : 0;
                            *src = srcVal << 1;
                            state->flags.z = !(*src);
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
                            DBG("%02X %02X: SRA %s\n", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            state->ClearFlags();
                            state->flags.c = srcVal & 0x01;
                            *src = (srcVal >> 1) | (srcVal & 0x80);
                            state->flags.z = !(*src);
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
                            DBG("%02X %02X: SWAP %s\n", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            state->ClearFlags();
                            *src = (srcVal << 4) | (srcVal >> 4);
                            state->flags.z = !(*src);
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
                            DBG("%02X %02X: SRL %s\n", opcode, subcode, srcStr);

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            state->ClearFlags();
                            state->flags.c = srcVal & 0x01;
                            *src = srcVal >> 1;
                            state->flags.z = !(*src);
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
                                state->timer->AddCycle();

                            uint8_t bit = (subcode >> 3) & 0x07;
                            
                            DBG("%02X %02X: BIT %d, %s\n", opcode, subcode, bit, srcStr);

                            // Carry bit not changed.
                            state->flags.z = !(*src & (1 << bit));
                            state->flags.n = 0;
                            state->flags.h = 1;
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
                            
                            DBG("%02X %02X: RES %d, %s\n", opcode, subcode, bit, srcStr);

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

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
                            
                            DBG("%02X %02X: SET %d, %s\n", opcode, subcode, bit, srcStr);

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

                            uint8_t srcVal = *src;

                            if (src->ExtraCycles())
                                state->timer->AddCycle();

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
                DBG("%02X %02X %02X: JP 0x%04X\n", opcode, LowByte(addr), HighByte(addr), addr);
                state->timer->AddCycle();
                state->pc = addr;
            }
            break;
        case 0xE9: // JP (HL)
            {
                DBG("%02X: JP (HL)\n", opcode);
                state->pc = state->hl;
            }
            break;
        case 0xC2: // JP NZ, nn
        case 0xCA: // JP Z, nn
        case 0xD2: // JP NC, nn
        case 0xDA: // JP C, nn
            {
                uint16_t addr = ReadPC16Bit();
                uint8_t flagBits = (opcode >> 3) & 0x03;
                DBG("%02X %02X %02X: JP %s, 0x%04X\n", opcode, LowByte(addr), HighByte(addr), flagNameMap[flagBits], addr);
                if (GetFlagValue(flagBits))
                {
                    state->pc = addr;
                    state->timer->AddCycle();
                }
            }
            break;

        case 0x18: // JR e
            {
                int8_t offset = ReadPC8Bit();
                DBG("%02X %02X: JR %d\n", opcode, offset, offset);
                if (offset == -2)
                {
                    DBG("Infinite loop detected\n");
                    exit(1);
                }
                state->pc += offset;
                state->timer->AddCycle();
            }
            break;
        case 0x20: // JR NZ, e
        case 0x28: // JR Z, e
        case 0x30: // JR NC, e
        case 0x38: // JR C, e
            {
                int8_t offset = ReadPC8Bit();
                uint8_t flagBits = (opcode >> 3) & 0x03;
                DBG("%02X %02X: JR %s, %d\n", opcode, (uint8_t)offset, flagNameMap[flagBits], offset);
                if (GetFlagValue(flagBits))
                {
                    if (offset == -2)
                    {
                        DBG("Infinite loop detected\n");
                        exit(1);
                    }
                    state->pc += offset;
                    state->timer->AddCycle();
                }
            }
            break;

        case 0xCD: // CALL nn
            {
                uint16_t addr = ReadPC16Bit();
                DBG("%02X %02X %02X: CALL %04X\n", opcode, LowByte(addr), HighByte(addr), addr);
                state->timer->AddCycle();
                Push(state->pc);
                state->pc = addr;
            }
            break;
        case 0xC4: // CALL NZ, nn
        case 0xCC: // CALL Z, nn
        case 0xD4: // CALL NC, nn
        case 0xDC: // CALL C, nn
            {
                uint16_t addr = ReadPC16Bit();
                uint8_t flagBits = (opcode >> 3) & 0x03;
                DBG("%02X %02X %02X: CALL %s, %04X\n", opcode, LowByte(addr), HighByte(addr), flagNameMap[flagBits], addr);
                if (GetFlagValue(flagBits))
                {
                    state->timer->AddCycle();
                    Push(state->pc);
                    state->pc = addr;
                }
            }
            break;

        case 0xC9: // RET
            {
                DBG("%02X: RET\n", opcode);
                state->timer->AddCycle();
                Pop(&state->pc);
            }
            break;
        case 0xD9: // RETI
            {
                DBG("%02X: RETI\n", opcode);
                state->timer->AddCycle();
                Pop(&state->pc);
                state->interrupts->SetEnabled(true);
            }
            break;
        case 0xC0: // RET NZ
        case 0xC8: // RET Z
        case 0xD0: // RET NC
        case 0xD8: // RET C
            {
                state->timer->AddCycle();
                uint8_t flagBits = (opcode >> 3) & 0x03;
                DBG("%02X: RET %s\n", opcode, flagNameMap[flagBits]);
                if (GetFlagValue(flagBits))
                {
                    Pop(&state->pc);
                    state->timer->AddCycle();
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
                DBG("%02X: RST 0x%02X\n", opcode, addr);
                state->timer->AddCycle();
                Push(state->pc);
                state->pc = (uint16_t)addr;
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                   //
// Misc opcodes                                                                                                      //
//                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x00: // NOP
            {
                DBG("%02X: NOP\n", opcode);
            }
            break;

        case 0x76: // HALT
            {
                DBG("%02X: HALT\n", opcode);
                eInterruptTypes intType;
                if (!state->interrupts->Enabled() && state->interrupts->CheckInterrupts(intType))
                    haltBug = true;
                else
                    state->halted = true;
            }
            break;

        case 0x37: // SCF
            {
                DBG("%02X: SCF\n", opcode);
                state->flags.c = 1;
            }
            break;

        case 0x3F: // CCF
            {
                DBG("%02X: CCF\n", opcode);
                state->flags.c = !state->flags.c;
            }
            break;

        case 0x27: // DAA
            {
                DBG("%02X: DAA\n", opcode);

                if (!state->flags.n)
                {
                    if (state->flags.c || state->a > 0x99)
                    {
                        state->a += 0x60;
                        state->flags.c = 1;
                    }
                    if (state->flags.h || (state->a & 0x0F) > 0x09)
                    {
                        state->a += 0x06;
                        state->flags.h = 0;
                    }
                }
                else
                {
                    if (state->flags.c && state->flags.h)
                    {
                        state->a += 0x9A;
                        state->flags.h = 0;
                    }
                    else if (state->flags.c)
                    {
                        state->a += 0xA0;
                    }
                    else if (state->flags.h)
                    {
                        state->a += 0xFA;
                        state->flags.h = 0;
                    }
                }

                state->flags.z = !((bool)state->a);
            }
            break;

        case 0xF3: // DI
            {
                DBG("%02X: DI\n", opcode);
                state->interrupts->SetEnabled(false);
            }
            break;

        case 0xFB: // EI
            {
                DBG("%02X: EI\n", opcode);

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