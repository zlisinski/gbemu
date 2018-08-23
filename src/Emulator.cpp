#include "gbemu.h"
#include "Emulator.h"

const char *Emulator::regNameMap8Bit[8] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
const char *Emulator::regNameMap16Bit[4] = {"BC", "DE", "HL", "SP"};
const char *Emulator::regNameMap16BitStack[4] = {"BC", "DE", "HL", "AF"};


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

    if ((opcode & 0xC0) == 0x40 && opcode != 0x76)
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
    else if ((opcode & 0xC7) == 0x06)
    {
        // 0x[0123][6E] is 8bit LD register, immediate
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
    else if ((opcode & 0xCF) == 0x01)
    {
        // 0x[0123]1 is 16bit LD register, immediate
        uint8_t destRegBits = (opcode >> 4) & 0x03;
        uint16_t *dest = regMap16Bit[destRegBits];
        uint16_t x = Read16bit();
        const char *destStr = regNameMap16Bit[destRegBits];
        
        DBG("%02X %02X %02X: LD %s, %04X\n", opcode, LowByte(x), HighByte(x), destStr, x);

        *dest = x;

        cycles = 12;
    }
    else if ((opcode & 0xCF) == 0xC5)
    {
        // 0x[CDEF]5 is PUSH register16
        uint8_t regBits = (opcode >> 4) & 0x03;
        uint16_t *src = regMap16BitStack[regBits];
        const char *srcStr = regNameMap16BitStack[regBits];

        DBG("%02X: PUSH %s\n", opcode, srcStr);

        Push(*src);

        cycles = 16;
    }
    else if ((opcode & 0xCF) == 0xC1)
    {
        // 0x[CDEF]1 is POP register16
        uint8_t regBits = (opcode >> 4) & 0x03;
        uint16_t *dest = regMap16BitStack[regBits];
        const char *destStr = regNameMap16BitStack[regBits];

        DBG("%02X: POP %s\n", opcode, destStr);

        Pop(dest);

        cycles = 12;
    }
    else if ((opcode & 0xF8) == 0x80)
    {
        // 0x8[0-7] is ADD A, register
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
    else if ((opcode & 0xF8) == 0x88)
    {
        // 0x8[8-F] is ADC A, register
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
    else if ((opcode & 0xF8) == 0x90)
    {
        // 0x9[0-7] is SUB A, register
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
    else if ((opcode & 0xF8) == 0x98)
    {
        // 0x9[8-F] is SBC A, register
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
    else if ((opcode & 0xF8) == 0xA0)
    {
        // 0xA[0-7] is AND A, register
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
    else if ((opcode & 0xF8) == 0xA8)
    {
        // 0xA[8-F] is XOR A, register
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
    else if ((opcode & 0xF8) == 0xB0)
    {
        // 0xB[8-F] is OR A, register
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

        state->a = state->a | *src;

        state->ClearFlags();
        if (state->a == 0)
            state->flags.z = 1;
    }
    else if ((opcode & 0xF8) == 0xB8)
    {
        // 0xB[8-F] is CP A, register
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

        //uint8_t oldA = state->a;
        // Subtract and don't save result.
        Sub8Bit(state->a, *src);
        //state->a = oldA;
    }
    else if ((opcode & 0xC7) == 0x04)
    {
        // 0x[0123][4C] is 8bit INC register
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
    else if ((opcode & 0xC7) == 0x05)
    {
        // 0x[0123][5D] is 8bit DEC register
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
    else if ((opcode & 0xCF) == 0x09)
    {
        // 0x[0123]9 is ADD HL, register16
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
    else if ((opcode & 0xCF) == 0x03)
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
    else if ((opcode & 0xCF) == 0x0B)
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
    else
    {
        switch (opcode)
        {
            case 0x00: NotYetImplemented(); break;
            //case 0x01: NotYetImplemented(); break;
            case 0x02:
                {
                    DBG("%02X: LD (BC), A\n", opcode);
                    state->memory[state->bc] = state->a;
                    cycles = 8;
                }
                break;
            //case 0x03: NotYetImplemented(); break;
            //case 0x04: NotYetImplemented(); break;
            //case 0x05: NotYetImplemented(); break;
            //case 0x06: NotYetImplemented(); break;
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
            //case 0x09: NotYetImplemented(); break;
            case 0x0A:
                {
                    DBG("LD A, (BC)\n");
                    state->a = state->memory[state->bc];
                    cycles = 8;
                }
                break;
            //case 0x0B: NotYetImplemented(); break;
            //case 0x0C: NotYetImplemented(); break;
            //case 0x0D: NotYetImplemented(); break;
            //case 0x0E: NotYetImplemented(); break;
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
            //case 0x11: NotYetImplemented(); break;
            case 0x12:
                {
                    DBG("LD (DE), A\n");
                    state->memory[state->de] = state->a;
                    cycles = 8;
                }
                break;
            //case 0x13: NotYetImplemented(); break;
            //case 0x14: NotYetImplemented(); break;
            //case 0x15: NotYetImplemented(); break;
            //case 0x16: NotYetImplemented(); break;
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
            case 0x18:
                {
                    int8_t offset = Read8bit();
                    DBG("JR %d\n", offset);
                    state->pc += offset;
                    cycles = 12;
                }
                break;
            //case 0x19: NotYetImplemented(); break;
            case 0x1A:
                {
                    DBG("LD A, (DE)\n");
                    state->a = state->memory[state->de];
                    cycles = 8;
                }
                break;
            //case 0x1B: NotYetImplemented(); break;
            //case 0x1C: NotYetImplemented(); break;
            //case 0x1D: NotYetImplemented(); break;
            //case 0x1E: NotYetImplemented(); break;
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

            case 0x20:
                {
                    int8_t offset = Read8bit();
                    DBG("JR NZ, %d\n", offset);
                    if (state->flags.z == 0)
                    {
                        state->pc += offset;
                        cycles = 12;
                    }
                    else
                        cycles = 8;
                }
                break;
            //case 0x21: NotYetImplemented(); break;
            case 0x22: // LD (HL+), A
                {
                    DBG("LD (HL+), A\n");
                    state->memory[state->hl] = state->a;
                    state->hl++;
                    cycles = 8;
                }
                break;
            //case 0x23: NotYetImplemented(); break;
            //case 0x24: NotYetImplemented(); break;
            //case 0x25: NotYetImplemented(); break;
            //case 0x26: NotYetImplemented(); break;
            case 0x27: NotYetImplemented(); break;
            case 0x28:
                {
                    int8_t offset = Read8bit();
                    DBG("JR Z, %d\n", offset);
                    if (state->flags.z == 1)
                    {
                        state->pc += offset;
                        cycles = 12;
                    }
                    else
                        cycles = 8;
                }
                break;
            //case 0x29: NotYetImplemented(); break;
            case 0x2A: // LD A, (HL+)
                {
                    DBG("LD A, (HL+)\n");
                    state->a = state->memory[state->hl];
                    state->hl++;
                    cycles = 8;
                }
                break;
            //case 0x2B: NotYetImplemented(); break;
            //case 0x2C: NotYetImplemented(); break;
            //case 0x2D: NotYetImplemented(); break;
            //case 0x2E: NotYetImplemented(); break;
            case 0x2F: NotYetImplemented(); break;

            case 0x30:
                {
                    int8_t offset = Read8bit();
                    DBG("JR NC, %d\n", offset);
                    if (state->flags.c == 0)
                    {
                        state->pc += offset;
                        cycles = 12;
                    }
                    else
                        cycles = 8;
                }
                break;
            //case 0x31: NotYetImplemented(); break;
            case 0x32: // LD (HL-), A
                {
                    DBG("LD (HL-), A\n");
                    state->memory[state->hl] = state->a;
                    state->hl--;
                    cycles = 8;
                }
                break;
            //case 0x33: NotYetImplemented(); break;
            //case 0x34: NotYetImplemented(); break;
            //case 0x35: NotYetImplemented(); break;
            //case 0x36: NotYetImplemented(); break;
            case 0x37: NotYetImplemented(); break;
            case 0x38:
                {
                    int8_t offset = Read8bit();
                    DBG("JR C, %d\n", offset);
                    if (state->flags.c == 1)
                    {
                        state->pc += offset;
                        cycles = 12;
                    }
                    else
                        cycles = 8;
                }
                break;
            //case 0x39: NotYetImplemented(); break;
            case 0x3A: // LD A, (HL-)
                {
                    DBG("LD A, (HL-)\n");
                    state->a = state->memory[state->hl];
                    state->hl--;
                    cycles = 8;
                }
                break;
            //case 0x3B: NotYetImplemented(); break;
            //case 0x3C: NotYetImplemented(); break;
            //case 0x3D: NotYetImplemented(); break;
            //case 0x3E: NotYetImplemented(); break;
            case 0x3F: NotYetImplemented(); break;


            case 0xC0:
                {
                    DBG("%02X: RET NZ\n", opcode);
                    if (!state->flags.z)
                    {
                        Pop(&state->pc);
                        cycles = 20;
                    }
                    else
                        cycles = 8;
                }
                break;
            //case 0xC1: NotYetImplemented(); break;
            case 0xC2:
                {
                    uint16_t x = Read16bit();
                    DBG("%02X %02X %02X: JP NZ, 0x%04X\n", opcode, LowByte(x), HighByte(x), x);

                    if (!state->flags.z)
                    {
                        state->pc = x;
                        cycles = 16;
                    }
                    else
                    {
                        cycles = 12;
                    }
                }
                break;
            case 0xC3:
                {
                    uint16_t x = Read16bit();
                    DBG("%02X %02X %02X: JP 0x%04X\n", opcode, LowByte(x), HighByte(x), x);
                    state->pc = x;
                    cycles = 16;
                }
                break;
            case 0xC4:
                {
                    uint16_t x = Read16bit();
                    DBG("%02X %02X %02X: CALL NZ, %04X\n", opcode, LowByte(x), HighByte(x), x);
                    if (!state->flags.z)
                    {
                        Push(state->pc);
                        state->pc = x;
                        cycles = 24;
                    }
                    else
                        cycles = 12;
                }
                break;
            //case 0xC5: NotYetImplemented(); break;
            case 0xC6:
                {
                    uint8_t x = Read8bit();
                    DBG("%02X %02X: ADD A, 0x%02X\n", opcode, x, x);
                    state->a = Add8Bit(state->a, x);
                    cycles = 8;
                }
                break;
            case 0xC7: NotYetImplemented(); break;
            case 0xC8:
                {
                    DBG("%02X: RET Z\n", opcode);
                    if (state->flags.z)
                    {
                        Pop(&state->pc);
                        cycles = 20;
                    }
                    else
                        cycles = 8;
                }
                break;
            case 0xC9:
                {
                    DBG("%02X: RET\n", opcode);
                    Pop(&state->pc);
                    cycles = 16;
                }
                break;
            case 0xCA:
                {
                    uint16_t x = Read16bit();
                    DBG("%02X %02X %02X: JP Z, 0x%04X\n", opcode, LowByte(x), HighByte(x), x);

                    if (state->flags.z)
                    {
                        state->pc = x;
                        cycles = 16;
                    }
                    else
                    {
                        cycles = 12;
                    }
                }
                break;
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
            case 0xCC:
                {
                    uint16_t x = Read16bit();
                    DBG("%02X %02X %02X: CALL Z, %04X\n", opcode, LowByte(x), HighByte(x), x);
                    if (state->flags.z)
                    {
                        Push(state->pc);
                        state->pc = x;
                        cycles = 24;
                    }
                    else
                        cycles = 12;
                }
                break;
            case 0xCD:
                {
                    uint16_t x = Read16bit();
                    DBG("%02X %02X %02X: CALL %04X\n", opcode, LowByte(x), HighByte(x), x);
                    Push(state->pc);
                    state->pc = x;
                    cycles = 24;
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

            case 0xD0:
                {
                    DBG("%02X: RET NC\n", opcode);
                    if (!state->flags.c)
                    {
                        Pop(&state->pc);
                        cycles = 20;
                    }
                    else
                        cycles = 8;
                }
                break;
            //case 0xD1: NotYetImplemented(); break;
            case 0xD2:
                {
                    uint16_t x = Read16bit();
                    DBG("%02X %02X %02X: JP NC, 0x%04X\n", opcode, LowByte(x), HighByte(x), x);

                    if (!state->flags.c)
                    {
                        state->pc = x;
                        cycles = 16;
                    }
                    else
                    {
                        cycles = 12;
                    }
                }
                break;
            case 0xD3: NotYetImplemented(); break;
            case 0xD4:
                {
                    uint16_t x = Read16bit();
                    DBG("%02X %02X %02X: CALL NC, %04X\n", opcode, LowByte(x), HighByte(x), x);
                    if (!state->flags.c)
                    {
                        Push(state->pc);
                        state->pc = x;
                        cycles = 24;
                    }
                    else
                        cycles = 12;
                }
                break;
            //case 0xD5: NotYetImplemented(); break;
            case 0xD6:
                {
                    uint8_t x = Read8bit();

                    DBG("%02X %02X: SUB A, 0x%02X\n", opcode, x, x);

                    state->a = Sub8Bit(state->a, x);
                    
                    cycles = 8;
                }
                 break;
            case 0xD7: NotYetImplemented(); break;
            case 0xD8:
                {
                    DBG("%02X: RET C\n", opcode);
                    if (state->flags.c)
                    {
                        Pop(&state->pc);
                        cycles = 20;
                    }
                    else
                        cycles = 8;
                }
                break;
            case 0xD9:
                {
                    DBG("%02X: RETI\n", opcode);
                    Pop(&state->pc);
                    cycles = 16;
                }
                break;
            case 0xDA:
                {
                    uint16_t x = Read16bit();
                    DBG("%02X %02X %02X: JP C, 0x%04X\n", opcode, LowByte(x), HighByte(x), x);

                    if (state->flags.c)
                    {
                        state->pc = x;
                        cycles = 16;
                    }
                    else
                    {
                        cycles = 12;
                    }
                }
                break;
            case 0xDB: NotYetImplemented(); break;
            case 0xDC:
                {
                    uint16_t x = Read16bit();
                    DBG("%02X %02X %02X: CALL C, %04X\n", opcode, LowByte(x), HighByte(x), x);
                    if (state->flags.c)
                    {
                        Push(state->pc);
                        state->pc = x;
                        cycles = 24;
                    }
                    else
                        cycles = 12;
                }
                break;
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
            //case 0xE1: NotYetImplemented(); break;
            case 0xE2:
                {
                    DBG("LD (0xFF00+C), A\n");
                    state->memory[0xFF00 + state->c] = state->a;
                    cycles = 8;
                }
                break;
            case 0xE3: NotYetImplemented(); break;
            case 0xE4: NotYetImplemented(); break;
            //case 0xE5: NotYetImplemented(); break;
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
            case 0xE9:
                {
                    DBG("%02X: JP (HL)\n", opcode);
                    state->pc = state->hl;
                    cycles = 4;
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
            //case 0xF1: NotYetImplemented(); break;
            case 0xF2:
                {
                    DBG("LD A, (0xFF00+C)\n");
                    state->a = state->memory[0xFF00 + state->c];
                    cycles = 8;
                }
                break;
            case 0xF3: NotYetImplemented(); break;
            case 0xF4: NotYetImplemented(); break;
            //case 0xF5: NotYetImplemented(); break;
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