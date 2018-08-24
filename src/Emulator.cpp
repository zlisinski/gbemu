#include "gbemu.h"
#include "Emulator.h"

const char *Emulator::regNameMap8Bit[8] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
const char *Emulator::regNameMap16Bit[4] = {"BC", "DE", "HL", "SP"};
const char *Emulator::regNameMap16BitStack[4] = {"BC", "DE", "HL", "AF"};
const char *Emulator::flagNameMap[4] = {"NZ", "Z", "NC", "C"};


Emulator::Emulator(State *state) : pc(NULL)
{
    this->state = state;

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
    printf("NYI opcode %02X at %04X\n", state->memory[state->pc], state->pc);
    exit(1);
}


inline uint8_t Emulator::Read8bit()
{
    uint8_t byte = state->memory[state->pc];
    state->pc++;
    return byte;
}


inline uint16_t Emulator::Read16bit()
{
    uint16_t word = (state->memory[state->pc + 1] << 8) | state->memory[state->pc];
    state->pc += 2;
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
    if ((result & 0xFF) == 0)
        state->flags.z = 1;

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
    if ((result & 0xFFFF) == 0)
        state->flags.z = 1;
    
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
    if ((result & 0xFF) == 0)
        state->flags.z = 1;

    return (result & 0xFF);
}


void Emulator::Push(uint16_t src)
{
    state->sp--;
    state->memory[state->sp] = (uint8_t)(src >> 8);
    state->sp--;
    state->memory[state->sp] = (uint8_t)(src & 0xFF);
}


void Emulator::Pop(uint16_t *dest)
{
    *dest = (state->memory[state->sp+1] << 8) + state->memory[state->sp];
    state->sp += 2;
}


int Emulator::ProcessOpCode()
{
    int cycles = 0;

    uint8_t opcode = Read8bit();
    DBG("opcode=%02X\n", opcode);

    switch (opcode)
    {
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47: // LD B, Register
        case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F: // LD C, Register
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57: // LD D, Register
        case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5E: case 0x5F: // LD E, Register
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67: // LD H, Register
        case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6E: case 0x6F: // LD L, Register
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: /*case 0x76:*/ case 0x77: // LD (HL), Register
        case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E: case 0x7F: // LD A, Register
            {
                // 0x40-0x7F, except 0x76, is LD register, register
                uint8_t destRegBits = (opcode >> 3) & 0x07;
                uint8_t srcRegBits = opcode & 0x07;
                uint8_t *dest = regMap8Bit[destRegBits];
                uint8_t *src = regMap8Bit[srcRegBits];
                const char *destStr = regNameMap8Bit[destRegBits];
                const char *srcStr = regNameMap8Bit[srcRegBits];
                cycles = 4;

                if (dest == NULL)
                {
                    dest = &state->memory[state->hl];
                    cycles = 8;
                }
                if (src == NULL)
                {
                    src = &state->memory[state->hl];
                    cycles = 8;
                }
                
                DBG("%02X: LD %s, %s\n", opcode, destStr, srcStr);

                *dest = *src;
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
                uint8_t *dest = regMap8Bit[destRegBits];
                uint8_t x = Read8bit();
                const char *destStr = regNameMap8Bit[destRegBits];
                cycles = 8;

                if (dest == NULL)
                {
                    dest = &state->memory[state->hl];
                    cycles = 12;
                }
                
                DBG("%02X %02X: LD %s, %02X\n", opcode, x, destStr, x);

                *dest = x;
            }
            break;

        case 0x01: // LD BC, immediate
        case 0x11: // LD DE, immediate
        case 0x21: // LD HL, immediate
        case 0x31: // LD SP, immediate
            {
                uint8_t destRegBits = (opcode >> 4) & 0x03;
                uint16_t *dest = regMap16Bit[destRegBits];
                uint16_t x = Read16bit();
                const char *destStr = regNameMap16Bit[destRegBits];
                
                DBG("%02X %02X %02X: LD %s, %04X\n", opcode, LowByte(x), HighByte(x), destStr, x);

                *dest = x;

                cycles = 12;
            }
            break;

        case 0xC5: // PUSH BC
        case 0xD5: // PUSH DE
        case 0xE5: // PUSH HL
        case 0xF5: // PUSH AF
            {
                uint8_t regBits = (opcode >> 4) & 0x03;
                uint16_t *src = regMap16BitStack[regBits];
                const char *srcStr = regNameMap16BitStack[regBits];

                DBG("%02X: PUSH %s\n", opcode, srcStr);

                Push(*src);

                cycles = 16;
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

                cycles = 12;
            }
            break;

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
                uint8_t *src = regMap8Bit[regBits];
                const char *srcStr = regNameMap8Bit[regBits];
                cycles = 4;

                if (src == NULL)
                {
                    src = &state->memory[state->hl];
                    cycles = 8;
                }

                DBG("%02X: ADD A, %s\n", opcode, srcStr);

                state->a = Add8Bit(state->a, *src);
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
                uint8_t *src = regMap8Bit[regBits];
                const char *srcStr = regNameMap8Bit[regBits];
                cycles = 4;

                if (src == NULL)
                {
                    src = &state->memory[state->hl];
                    cycles = 8;
                }

                DBG("%02X: ADC A, %s, %d\n", opcode, srcStr, state->flags.c);

                state->a = Add8Bit(state->a, *src, state->flags.c);
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
                uint8_t *src = regMap8Bit[regBits];
                const char *srcStr = regNameMap8Bit[regBits];
                cycles = 4;

                if (src == NULL)
                {
                    src = &state->memory[state->hl];
                    cycles = 8;
                }

                DBG("%02X: SUB A, %s\n", opcode, srcStr);

                state->a = Sub8Bit(state->a, *src);
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
                uint8_t *src = regMap8Bit[regBits];
                const char *srcStr = regNameMap8Bit[regBits];
                cycles = 4;

                if (src == NULL)
                {
                    src = &state->memory[state->hl];
                    cycles = 8;
                }

                DBG("%02X: SBC A, %s, %d\n", opcode, srcStr, state->flags.c);

                state->a = Sub8Bit(state->a, *src, state->flags.c);
            }
            break;

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
                uint8_t *src = regMap8Bit[regBits];
                const char *srcStr = regNameMap8Bit[regBits];
                cycles = 4;

                if (src == NULL)
                {
                    src = &state->memory[state->hl];
                    cycles = 8;
                }

                DBG("%02X: AND A, %s\n", opcode, srcStr);

                state->a = state->a & *src;

                state->ClearFlags();
                state->flags.h = 1;
                if (state->a == 0)
                    state->flags.z = 1;
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
                uint8_t *src = regMap8Bit[regBits];
                const char *srcStr = regNameMap8Bit[regBits];
                cycles = 4;

                if (src == NULL)
                {
                    src = &state->memory[state->hl];
                    cycles = 8;
                }

                DBG("%02X: XOR A, %s\n", opcode, srcStr);

                state->a = state->a ^ *src;

                state->ClearFlags();
                if (state->a == 0)
                    state->flags.z = 1;
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
                uint8_t *src = regMap8Bit[regBits];
                const char *srcStr = regNameMap8Bit[regBits];
                cycles = 4;

                if (src == NULL)
                {
                    src = &state->memory[state->hl];
                    cycles = 8;
                }

                DBG("%02X: OR A, %s\n", opcode, srcStr);

                state->a = state->a | *src;

                state->ClearFlags();
                if (state->a == 0)
                    state->flags.z = 1;
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
                uint8_t *src = regMap8Bit[regBits];
                const char *srcStr = regNameMap8Bit[regBits];
                cycles = 4;

                if (src == NULL)
                {
                    src = &state->memory[state->hl];
                    cycles = 8;
                }

                DBG("%02X: CP A, %s\n", opcode, srcStr);

                // Subtract and don't save result.
                Sub8Bit(state->a, *src);
            }
            break;

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
                uint8_t *src = regMap8Bit[regBits];
                const char *srcStr = regNameMap8Bit[regBits];
                cycles = 4;

                if (src == NULL)
                {
                    src = &state->memory[state->hl];
                    cycles = 12;
                }

                DBG("%02X: INC %s\n", opcode, srcStr);

                // Carry flag not changed.
                uint8_t oldCarry = state->flags.c;
                *src = Add8Bit(*src, 1);
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
                uint8_t *src = regMap8Bit[regBits];
                const char *srcStr = regNameMap8Bit[regBits];
                cycles = 4;

                if (src == NULL)
                {
                    src = &state->memory[state->hl];
                    cycles = 12;
                }

                DBG("%02X: DEC %s\n", opcode, srcStr);

                (*src)--;
                
                // Carry flag not changed.
                state->flags.n = 1;
                state->flags.z = (*src == 0) ? 1 : 0;
                state->flags.h = ((*src & 0x0F) == 0x0F) ? 1 : 0;
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
                cycles = 8;

                DBG("%02X: ADD HL, %s\n", opcode, srcStr);

                // Zero flag not changed.
                uint8_t oldZero = state->flags.z;
                state->hl = Add16Bit(state->hl, *src);
                state->flags.z = oldZero;
            }
            break;

        case 0x03: // INC BC
        case 0x13: // INC DE
        case 0x23: // INC HL
        case 0x33: // INC SP
            {
                // 0x[0123]3 is 16bit INC register
                uint8_t regBits = (opcode >> 4) & 0x03;
                uint16_t *src = regMap16Bit[regBits];
                const char *srcStr = regNameMap16Bit[regBits];
                cycles = 8;

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
                // 0x[0123]B is 16bit DEC register
                uint8_t regBits = (opcode >> 4) & 0x03;
                uint16_t *src = regMap16Bit[regBits];
                const char *srcStr = regNameMap16Bit[regBits];
                cycles = 8;

                DBG("%02X: DEC %s\n", opcode, srcStr);

                // Flags not changed.
                (*src)--;
            }
            break;



        case 0xC3: // JP nn
            {
                uint16_t addr = Read16bit();
                DBG("%02X %02X %02X: JP 0x%04X\n", opcode, LowByte(addr), HighByte(addr), addr);
                state->pc = addr;
                cycles = 16;
            }
            break;
        case 0xE9: // JP (HL)
            {
                DBG("%02X: JP (HL)\n", opcode);
                state->pc = state->hl;
                cycles = 4;
            }
            break;
        case 0xC2: // JP NZ, nn
        case 0xCA: // JP Z, nn
        case 0xD2: // JP NC, nn
        case 0xDA: // JP C, nn
            {
                uint16_t addr = Read16bit();
                uint8_t flagBits = (opcode >> 3) & 0x03;
                DBG("%02X %02X %02X: JP %s, 0x%04X\n", opcode, LowByte(addr), HighByte(addr), flagNameMap[flagBits], addr);
                if (GetFlagValue(flagBits))
                {
                    state->pc = addr;
                    cycles = 16;
                }
                else
                    cycles = 12;
            }
            break;

        case 0x18: // JR e
            {
                int8_t offset = Read8bit();
                DBG("%02X %02X: JR %d\n", opcode, offset, offset);
                state->pc += offset;
                cycles = 12;
            }
            break;
        case 0x20: // JR NZ, e
        case 0x28: // JR Z, e
        case 0x30: // JR NC, e
        case 0x38: // JR C, e
            {
                int8_t offset = Read8bit();
                uint8_t flagBits = (opcode >> 3) & 0x03;
                DBG("%02X %02X: JR %s, %d\n", opcode, (uint8_t)offset, flagNameMap[flagBits], offset);
                if (GetFlagValue(flagBits))
                {
                    state->pc += offset;
                    cycles = 12;
                }
                else
                    cycles = 8;
            }
            break;

        case 0xCD: // CALL nn
            {
                uint16_t addr = Read16bit();
                DBG("%02X %02X %02X: CALL %04X\n", opcode, LowByte(addr), HighByte(addr), addr);
                Push(state->pc);
                state->pc = addr;
                cycles = 24;
            }
            break;
        case 0xC4: // CALL NZ, nn
        case 0xCC: // CALL Z, nn
        case 0xD4: // CALL NC, nn
        case 0xDC: // CALL C, nn
            {
                uint16_t addr = Read16bit();
                uint8_t flagBits = (opcode >> 3) & 0x03;
                DBG("%02X %02X %02X: CALL %s, %04X\n", opcode, LowByte(addr), HighByte(addr), flagNameMap[flagBits], addr);
                if (GetFlagValue(flagBits))
                {
                    Push(state->pc);
                    state->pc = addr;
                    cycles = 24;
                }
                else
                    cycles = 12;
            }
            break;

        case 0xC9: // RET
            {
                DBG("%02X: RET\n", opcode);
                Pop(&state->pc);
                cycles = 16;
            }
            break;
        case 0xD9: // RETI
            {
                DBG("%02X: RETI\n", opcode);
                Pop(&state->pc);
                cycles = 16;
            }
            break;
        case 0xC0: // RET NZ
        case 0xC8: // RET Z
        case 0xD0: // RET NC
        case 0xD8: // RET C
            {
                uint8_t flagBits = (opcode >> 3) & 0x03;
                DBG("%02X: RET %s\n", opcode, flagNameMap[flagBits]);
                if (GetFlagValue(flagBits))
                {
                    Pop(&state->pc);
                    cycles = 20;
                }
                else
                    cycles = 8;
            }
            break;
        

        case 0x00: NotYetImplemented(); break;
        case 0x02:
            {
                DBG("%02X: LD (BC), A\n", opcode);
                state->memory[state->bc] = state->a;
                cycles = 8;
            }
            break;
        case 0x07:
            {
                DBG("%02X: RLCA\n", opcode);
                state->ClearFlags();
                state->flags.c = (state->a & 0x80) ? 1 : 0;
                state->a = (state->a << 1) | state->flags.c;
                cycles = 4;
            }
            break;
        case 0x08:
            {
                uint16_t x = Read16bit();
                DBG("%02X %02X %02X: LD (0x%04X), SP\n", opcode, HighByte(x), LowByte(x), x);

                state->memory[x] = (uint8_t)(state->sp & 0xff);
                state->memory[x+1] = (uint8_t)(state->sp >> 8);

                cycles = 20;
            }
            break;
        case 0x0A:
            {
                DBG("LD A, (BC)\n");
                state->a = state->memory[state->bc];
                cycles = 8;
            }
            break;
        case 0x0F:
            {
                DBG("%02X: RRCA\n", opcode);
                state->ClearFlags();
                state->flags.c = (state->a & 0x01) ? 1 : 0;
                state->a = (state->a >> 1) | (state->flags.c << 7);
                cycles = 4;
            }
            break;

        case 0x10: NotYetImplemented(); break;
        case 0x12:
            {
                DBG("LD (DE), A\n");
                state->memory[state->de] = state->a;
                cycles = 8;
            }
            break;
        case 0x17:
            {
                DBG("%02X: RLA\n", opcode);
                uint8_t oldCarry = state->flags.c;
                state->ClearFlags();
                state->flags.c = (state->a & 0x80) ? 1 : 0;
                state->a = (state->a << 1) | oldCarry;
                cycles = 4;
            }
            break;
        case 0x1A:
            {
                DBG("LD A, (DE)\n");
                state->a = state->memory[state->de];
                cycles = 8;
            }
            break;
        case 0x1F:
            {
                DBG("%02X: RRA\n", opcode);
                uint8_t oldCarry = state->flags.c;
                state->ClearFlags();
                state->flags.c = (state->a & 0x01) ? 1 : 0;
                state->a = (state->a >> 1) | (oldCarry << 7);
                cycles = 4;
            }
            break;

        case 0x22: // LD (HL+), A
            {
                DBG("LD (HL+), A\n");
                state->memory[state->hl] = state->a;
                state->hl++;
                cycles = 8;
            }
            break;
        case 0x27: NotYetImplemented(); break;
        case 0x2A: // LD A, (HL+)
            {
                DBG("LD A, (HL+)\n");
                state->a = state->memory[state->hl];
                state->hl++;
                cycles = 8;
            }
            break;
        case 0x2F: NotYetImplemented(); break;

        case 0x32: // LD (HL-), A
            {
                DBG("LD (HL-), A\n");
                state->memory[state->hl] = state->a;
                state->hl--;
                cycles = 8;
            }
            break;
        case 0x37: NotYetImplemented(); break;
        case 0x3A: // LD A, (HL-)
            {
                DBG("LD A, (HL-)\n");
                state->a = state->memory[state->hl];
                state->hl--;
                cycles = 8;
            }
            break;
        case 0x3F: NotYetImplemented(); break;

        case 0xC6:
            {
                uint8_t x = Read8bit();
                DBG("%02X %02X: ADD A, 0x%02X\n", opcode, x, x);
                state->a = Add8Bit(state->a, x);
                cycles = 8;
            }
            break;
        case 0xC7: NotYetImplemented(); break;
        case 0xCB:
            {
                uint8_t subcode = Read8bit();

                uint8_t regBits = subcode & 0x07;
                uint8_t *src = regMap8Bit[regBits];
                const char *srcStr = regNameMap8Bit[regBits];
                cycles = 8;

                if (src == NULL)
                {
                    src = &state->memory[state->hl];
                    cycles = 16;
                }

                if ((subcode & 0xF8) == 0x00)
                {
                    DBG("%02X %02X: RLC %s\n", opcode, subcode, srcStr);

                    state->ClearFlags();
                    state->flags.c = (*src & 0x80) ? 1 : 0;
                    *src = (*src << 1) | state->flags.c;
                    state->flags.z = *src == 0 ? 1 : 0;
                }
                else if ((subcode & 0xF8) == 0x08)
                {
                    DBG("%02X %02X: RRC %s\n", opcode, subcode, srcStr);

                    state->ClearFlags();
                    state->flags.c = *src & 0x01;
                    *src = (*src >> 1) | (state->flags.c << 7);
                    state->flags.z = *src == 0 ? 1 : 0;
                }
                else if ((subcode & 0xF8) == 0x10)
                {
                    DBG("%02X %02X: RL %s\n", opcode, subcode, srcStr);

                    uint8_t oldCarry = state->flags.c;
                    state->ClearFlags();
                    state->flags.c = (*src & 0x80) ? 1 : 0;
                    *src = (*src << 1) | oldCarry;
                    state->flags.z = *src == 0 ? 1 : 0;
                }
                else if ((subcode & 0xF8) == 0x18)
                {
                    DBG("%02X %02X: RR %s\n", opcode, subcode, srcStr);

                    uint8_t oldCarry = state->flags.c;
                    state->ClearFlags();
                    state->flags.c = *src & 0x01;
                    *src = (*src >> 1) | (oldCarry << 7);
                    state->flags.z = *src == 0 ? 1 : 0;
                }
                else if ((subcode & 0xF8) == 0x20)
                {
                    DBG("%02X %02X: SLA %s\n", opcode, subcode, srcStr);

                    state->ClearFlags();
                    state->flags.c = (*src & 0x80) ? 1 : 0;
                    *src <<= 1;
                    state->flags.z = *src == 0 ? 1 : 0;
                }
                else if ((subcode & 0xF8) == 0x28)
                {
                    DBG("%02X %02X: SRA %s\n", opcode, subcode, srcStr);

                    state->ClearFlags();
                    state->flags.c = *src & 0x01;
                    *src = (*src >> 1) | (*src & 0x80);
                    state->flags.z = *src == 0 ? 1 : 0;
                }
                else if ((subcode & 0xF8) == 0x30)
                {
                    DBG("%02X %02X: SWAP %s\n", opcode, subcode, srcStr);

                    state->ClearFlags();
                    *src = (*src << 4) | (*src >> 4);
                    state->flags.z = *src == 0 ? 1 : 0;
                }
                else if ((subcode & 0xF8) == 0x38)
                {
                    DBG("%02X %02X: SRL %s\n", opcode, subcode, srcStr);

                    state->ClearFlags();
                    state->flags.c = *src & 0x01;
                    *src >>= 1;
                    state->flags.z = *src == 0 ? 1 : 0;
                }
                else if ((subcode & 0xC0) == 0x40)
                {
                    uint8_t bit = (subcode >> 3) & 0x07;
                    
                    DBG("%02X %02X: BIT %d, %s\n", opcode, subcode, bit, srcStr);

                    // Carry bit not changed.
                    state->flags.z = (*src & (1 << bit)) ? 0 : 1;
                    state->flags.n = 0;
                    state->flags.h = 1;
                }
                else if ((subcode & 0xC0) == 0x80)
                {
                    uint8_t bit = (subcode >> 3) & 0x07;
                    
                    DBG("%02X %02X: RES %d, %s\n", opcode, subcode, bit, srcStr);

                    *src &= ~(1 << bit);
                }
                else if ((subcode & 0xC0) == 0xC0)
                {
                    uint8_t bit = (subcode >> 3) & 0x07;
                    
                    DBG("%02X %02X: SET %d, %s\n", opcode, subcode, bit, srcStr);

                    *src |= (1 << bit);
                }
            }
            break;
        case 0xCE:
            {
                uint8_t x = Read8bit();
                DBG("%02X %02X: ADC A, 0x%02X, %d\n", opcode, x, x, state->flags.c);
                state->a = Add8Bit(state->a, x, state->flags.c);
                cycles = 8;
            }
            break;
        case 0xCF: NotYetImplemented(); break;

        case 0xD3: NotYetImplemented(); break;
        case 0xD6:
            {
                uint8_t x = Read8bit();

                DBG("%02X %02X: SUB A, 0x%02X\n", opcode, x, x);

                state->a = Sub8Bit(state->a, x);
                
                cycles = 8;
            }
                break;
        case 0xD7: NotYetImplemented(); break;
        case 0xDB: NotYetImplemented(); break;
        case 0xDD: NotYetImplemented(); break;
        case 0xDE:
            {
                uint8_t x = Read8bit();
                DBG("%02X %02X: SBC A, 0x%02X, %d\n", opcode, x, x, state->flags.c);
                state->a = Sub8Bit(state->a, x, state->flags.c);
                cycles = 8;
            }
            break;
        case 0xDF: NotYetImplemented(); break;

        case 0xE0:
            {
                uint8_t x = Read8bit();
                DBG("LD (0xFF00+0x%02X), A\n", x);
                state->memory[0xFF00 + x] = state->a;
                cycles = 12;
            }
            break;
        case 0xE2:
            {
                DBG("LD (0xFF00+C), A\n");
                state->memory[0xFF00 + state->c] = state->a;
                cycles = 8;
            }
            break;
        case 0xE3: NotYetImplemented(); break;
        case 0xE4: NotYetImplemented(); break;
        case 0xE6:
            {
                uint8_t x = Read8bit();

                DBG("%02X %02X: AND A, 0x%02X\n", opcode, x, x);

                state->a = state->a & x;
                
                cycles = 8;

                state->ClearFlags();
                state->flags.h = 1;
                if (state->a == 0)
                    state->flags.z = 1;
            }
            break;
        case 0xE7: NotYetImplemented(); break;
        case 0xE8:
            {
                uint8_t x = Read8bit();

                DBG("%02X %02X: ADD SP, %02X\n", opcode, x, x);

                state->sp = Add16Bit(state->sp, x);

                // Zero flag is always cleared.
                state->flags.z = 0;

                cycles = 16;
            }
            break;
        case 0xEA:
            {
                uint16_t x = Read16bit();
                DBG("LD (%04X), A\n", x);
                state->memory[x] = state->a;
                cycles = 16;
            }
            break;
        case 0xEB: NotYetImplemented(); break;
        case 0xEC: NotYetImplemented(); break;
        case 0xED: NotYetImplemented(); break;
        case 0xEE:
            {
                uint8_t x = Read8bit();

                DBG("%02X %02X: XOR A, 0x%02X\n", opcode, x, x);

                state->a = state->a ^ x;
                
                cycles = 8;

                state->ClearFlags();
                if (state->a == 0)
                    state->flags.z = 1;
            }
            break;
        case 0xEF: NotYetImplemented(); break;

        case 0xF0:
            {
                uint8_t x = Read8bit();
                DBG("LD A, (0xFF00+0x%02X)\n", x);
                state->a = state->memory[0xFF00 + x];
                cycles = 12;
            }
            break;
        case 0xF2:
            {
                DBG("LD A, (0xFF00+C)\n");
                state->a = state->memory[0xFF00 + state->c];
                cycles = 8;
            }
            break;
        case 0xF3: NotYetImplemented(); break;
        case 0xF4: NotYetImplemented(); break;
        case 0xF6:
            {
                uint8_t x = Read8bit();

                DBG("%02X %02X: OR A, 0x%02X\n", opcode, x, x);

                state->a = state->a | x;
                
                cycles = 8;

                state->ClearFlags();
                if (state->a == 0)
                    state->flags.z = 1;
            }
            break;
        case 0xF7: NotYetImplemented(); break;
        case 0xF8:
            {
                int8_t x = Read8bit();

                DBG("LD HL, SP+0x%02X\n", x);

                uint16_t result = Add16Bit(state->sp, x);
                state->hl = result;

                // Zero flag is always cleared.
                state->flags.z = 0;
                
                cycles = 12;
            }
            break;
        case 0xF9:
            {
                DBG("LD SP, HL\n");
                state->sp = state->hl;
                cycles = 8;
            }
            break;
        case 0xFA:
            {
                uint16_t x = Read16bit();
                DBG("LD A, (%04X)\n", x);
                state->a = state->memory[x];
                cycles = 16;
            }
            break;
        case 0xFB: NotYetImplemented(); break;
        case 0xFC: NotYetImplemented(); break;
        case 0xFD: NotYetImplemented(); break;
        case 0xFE:
            {
                uint8_t x = Read8bit();

                DBG("%02X %02X: CP A, 0x%02X\n", opcode, x, x);
                
                cycles = 8;

                //uint8_t oldA = state->a;
                // Subtract and don't save result.
                Sub8Bit(state->a, x);
                //state->a = oldA;
            }
            break;
        case 0xFF: NotYetImplemented(); break;

        default: NotYetImplemented(); break;
    }

    state->PrintState();

    if (cycles == 0)
    {
        printf("Missing cycles\n");
        exit(1);
    }

    DBG("\n");

    return cycles;
}