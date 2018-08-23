#include "EmulatorTest.h"
#include "../State.h"
#include "../Emulator.h"

const uint8_t A_VALUE = 0x12;
const uint8_t F_VALUE = 0x00;
const uint8_t B_VALUE = 0x34;
const uint8_t C_VALUE = 0x56;
const uint8_t D_VALUE = 0x78;
const uint8_t E_VALUE = 0x9A;
const uint8_t H_VALUE = 0xBC;
const uint8_t L_VALUE = 0xDE;
const uint16_t AF_VALUE = 0x1200;
const uint16_t BC_VALUE = 0x3456;
const uint16_t DE_VALUE = 0x789A;
const uint16_t HL_VALUE = 0xBCDE;
const uint16_t SP_VALUE = 0xFFFE;


EmulatorTest::EmulatorTest()
{

}

EmulatorTest::~EmulatorTest()
{

}

void EmulatorTest::SetUp()
{
    ResetState();
}

void EmulatorTest::TearDown()
{

}

void EmulatorTest::ResetState()
{
    state.af = AF_VALUE;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;

    memset(state.memory, 0, MEM_SIZE);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, TEST_Add8Bit)
{
    uint8_t result;
    Emulator emulator(&state);

    result = emulator.Add8Bit(0x00, 0x00);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add8Bit(0x00, 0x01);
    ASSERT_EQ(result, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add8Bit(0x01, 0x01);
    ASSERT_EQ(result, 2);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add8Bit(0x01, 0x01, 1);
    ASSERT_EQ(result, 3);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add8Bit(0x0F, 0x01);
    ASSERT_EQ(result, 0x10);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add8Bit(0x0E, 0x01, 1);
    ASSERT_EQ(result, 0x10);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add8Bit(0x0F, 0x0F);
    ASSERT_EQ(result, 0x1E);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add8Bit(0xF1, 0xFE);
    ASSERT_EQ(result, 0xEF);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add8Bit(0xFF, 0xFF);
    ASSERT_EQ(result, 0xFE);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add8Bit(0xA5, 0x5A);
    ASSERT_EQ(result, 0xFF);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add8Bit(0xA5, 0x5A, 1);
    ASSERT_EQ(result, 0x00);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add8Bit(0xFF, 0x01);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.n, 0);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, TEST_Add16Bit)
{
    uint16_t result;
    Emulator emulator(&state);

    result = emulator.Add16Bit(0x00, 0x00);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add16Bit(0x00, 0x01);
    ASSERT_EQ(result, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add16Bit(0x01, 0x01);
    ASSERT_EQ(result, 2);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add16Bit(0x0F00, 0x0100);
    ASSERT_EQ(result, 0x1000);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add16Bit(0x0F00, 0x0F00);
    ASSERT_EQ(result, 0x1E00);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add16Bit(0xF100, 0xFE00);
    ASSERT_EQ(result, 0xEF00);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add16Bit(0xFFFF, 0xFFFF);
    ASSERT_EQ(result, SP_VALUE);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add16Bit(0xA5A5, 0x5A5A);
    ASSERT_EQ(result, 0xFFFF);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add16Bit(0xFFFF, 0x0001);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.n, 0);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, TEST_Sub8Bit)
{
    uint16_t result;
    Emulator emulator(&state);

    result = emulator.Sub8Bit(0x00, 0x00);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 1);

    ResetState();
    result = emulator.Sub8Bit(0x01, 0x00);
    ASSERT_EQ(result, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 1);

    ResetState();
    result = emulator.Sub8Bit(0x01, 0x00, 1);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 1);

    ResetState();
    result = emulator.Sub8Bit(0x11, 0x02);
    ASSERT_EQ(result, 0x0F);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 1);

    ResetState();
    result = emulator.Sub8Bit(0x11, 0x01);
    ASSERT_EQ(result, 0x10);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 1);

    ResetState();
    result = emulator.Sub8Bit(0x11, 0x01, 1);
    ASSERT_EQ(result, 0x0F);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 1);

    ResetState();
    result = emulator.Sub8Bit(0xA5, 0x5A);
    ASSERT_EQ(result, 0x4B);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 1);

    ResetState();
    result = emulator.Sub8Bit(0x01, 0xFF);
    ASSERT_EQ(result, 0x02);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.n, 1);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_REG_REG_8Bit)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD B. B
    ResetState();
    state.memory[0] = 0x40;
    state.b = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.b, 0);
    ASSERT_EQ(state.c, C_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // LD B. C
    ResetState();
    state.memory[0] = 0x41;
    state.c = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.b, 0);
    ASSERT_EQ(state.c, 0);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // LD B. D
    ResetState();
    state.memory[0] = 0x42;
    state.d = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.b, 0);
    ASSERT_EQ(state.c, C_VALUE);
    ASSERT_EQ(state.d, 0);
    ASSERT_EQ(state.e, E_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // LD B. E
    ResetState();
    state.memory[0] = 0x43;
    state.e = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.b, 0);
    ASSERT_EQ(state.c, C_VALUE);
    ASSERT_EQ(state.d, D_VALUE);
    ASSERT_EQ(state.e, 0);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // LD B. H
    ResetState();
    state.memory[0] = 0x44;
    state.h = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.b, 0);
    ASSERT_EQ(state.c, C_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.h, 0);
    ASSERT_EQ(state.l, L_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // LD B. L
    ResetState();
    state.memory[0] = 0x45;
    state.l = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.b, 0);
    ASSERT_EQ(state.c, C_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.h, H_VALUE);
    ASSERT_EQ(state.l, 0);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // LD B. (HL)
    ResetState();
    state.memory[0] = 0x46;
    state.memory[HL_VALUE] = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.b, 0);
    ASSERT_EQ(state.c, C_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);

    // LD B. A
    ResetState();
    state.memory[0] = 0x47;
    state.a = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.b, 0);
    ASSERT_EQ(state.c, C_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // TODO: Add the other 56 opcodes

    // LD (HL), B
    ResetState();
    state.memory[0] = 0x70;
    state.memory[HL_VALUE] = 1;
    state.b = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 1);
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.b, 1);
    ASSERT_EQ(state.c, C_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_REG_NN_8Bit)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD B. NN
    ResetState();
    state.memory[0] = 0x06;
    state.memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.b, 1);
    ASSERT_EQ(state.c, C_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(cycles, 8);

    // LD C. NN
    ResetState();
    state.memory[0] = 0x0E;
    state.memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.b, B_VALUE);
    ASSERT_EQ(state.c, 1);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(cycles, 8);

    // LD D. NN
    ResetState();
    state.memory[0] = 0x16;
    state.memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.d, 1);
    ASSERT_EQ(state.e, E_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(cycles, 8);

    // LD E. NN
    ResetState();
    state.memory[0] = 0x1E;
    state.memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.d, D_VALUE);
    ASSERT_EQ(state.e, 1);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(cycles, 8);

    // LD H. NN
    ResetState();
    state.memory[0] = 0x26;
    state.memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.h, 1);
    ASSERT_EQ(state.l, L_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(cycles, 8);

    // LD L. NN
    ResetState();
    state.memory[0] = 0x2E;
    state.memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.h, H_VALUE);
    ASSERT_EQ(state.l, 1);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(cycles, 8);

    // LD (HL). NN
    ResetState();
    state.memory[0] = 0x36;
    state.memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 1);
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(cycles, 12);

    // LD A. NN
    ResetState();
    state.memory[0] = 0x3E;
    state.memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_A_BC)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD A, (BC)
    ResetState();
    state.memory[0] = 0x0A;
    state.memory[BC_VALUE] = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_A_DE)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD A, (DE)
    ResetState();
    state.memory[0] = 0x1A;
    state.memory[DE_VALUE] = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_A_0xFF00_PLUS_C)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD A, (0xFF00+C)
    ResetState();
    state.memory[0] = 0xF2;
    state.memory[0xFF00 + C_VALUE] = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_0xFF00_PLUS_C_A)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD (0xFF00+C), A
    ResetState();
    state.memory[0] = 0xE2;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[0xFF00 + C_VALUE], 1);
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_A_0xFF00_PLUS_N)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD A, (0xFF00+N)
    ResetState();
    state.memory[0] = 0xF0;
    state.memory[1] = 0x02;
    state.memory[0xFF02] = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_0xFF00_PLUS_N_A)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD (0xFF00+N), A
    ResetState();
    state.memory[0] = 0xE0;
    state.memory[1] = 0x02;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[0xFF02], 1);
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_A_NN)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD A, (NN)
    ResetState();
    state.memory[0] = 0xFA;
    state.memory[1] = 0x02;
    state.memory[2] = 0x01;
    state.memory[0x0102] = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_NN_A)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD (NN), A
    ResetState();
    state.memory[0] = 0xEA;
    state.memory[1] = 0x02;
    state.memory[2] = 0x01;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[0x0102], 1);
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_A_HL_PLUS)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD A, (HL+)
    ResetState();
    state.memory[0] = 0x2A;
    state.memory[HL_VALUE] = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE + 1);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_A_HL_MINUS)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD A, (HL-)
    ResetState();
    state.memory[0] = 0x3A;
    state.memory[HL_VALUE] = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE - 1);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_BC_A)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD (BC), A
    ResetState();
    state.memory[0] = 0x02;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[BC_VALUE], 1);
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_DE_A)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD (DE), A
    ResetState();
    state.memory[0] = 0x12;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[DE_VALUE], 1);
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_HLI_A)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD (HL+), A
    ResetState();
    state.memory[0] = 0x22;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 1);
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE + 1);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_HLD_A)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD (HL-), A
    ResetState();
    state.memory[0] = 0x32;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 1);
    ASSERT_EQ(state.a, 1);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE - 1);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_REG_NN_16Bit)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD BC. NN
    ResetState();
    state.memory[0] = 0x01;
    state.memory[1] = 0x01;
    state.memory[2] = 0x02;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.bc, 0x0201);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(cycles, 12);

    // LD DE. NN
    ResetState();
    state.memory[0] = 0x11;
    state.memory[1] = 0x01;
    state.memory[2] = 0x02;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, 0x0201);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(cycles, 12);

    // LD HL. NN
    ResetState();
    state.memory[0] = 0x21;
    state.memory[1] = 0x01;
    state.memory[2] = 0x02;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, 0x0201);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(cycles, 12);

    // LD SP. NN
    ResetState();
    state.memory[0] = 0x31;
    state.memory[1] = 0x01;
    state.memory[2] = 0x02;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, 0x0201);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_SP_HL)
{
    Emulator emulator(&state);
    int cycles = 0;

    // LD SP, HL
    ResetState();
    state.memory[0] = 0xF9;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.f, F_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_HL_SP_PLUS_NN)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.sp = 0;
    state.memory[0] = 0xF8;
    state.memory[1] = 0x0F;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.hl, 0x000F);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.sp, 0);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    // Half-carry is on bit 11->12
    ResetState();
    state.sp = 0x000F;
    state.memory[0] = 0xF8;
    state.memory[1] = 0x0F;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.hl, 0x001E);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.sp, 0x000F);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    // Half-carry is on bit 11->12
    ResetState();
    state.sp = 0x0FFF;
    state.memory[0] = 0xF8;
    state.memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.hl, 0x1000);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.sp, 0x0FFF);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    // Test carry and half-carry
    ResetState();
    state.sp = 0xFFFF;
    state.memory[0] = 0xF8;
    state.memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.hl, 0x0000);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.sp, 0xFFFF);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_LD_NN_SP)
{
    Emulator emulator(&state);
    int cycles = 0;

    state.memory[0] = 0x08;
    state.memory[1] = 0x00;
    state.memory[2] = 0xC1;
    state.sp = 0xFFF8;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[0xC100], 0xF8);
    ASSERT_EQ(state.memory[0xC101], 0xFF);
    ASSERT_EQ(state.a, 0x12);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, 0xFFF8);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 20);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_PUSH)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.a = 0x12;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    state.memory[0] = 0xC5;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(state.memory[0xFFFD], state.b);
    ASSERT_EQ(state.memory[0xFFFC], state.c);
    ASSERT_EQ(state.a, 0x12);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    state.pc = 0;
    state.memory[0] = 0xD5;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, 0xFFFA);
    ASSERT_EQ(state.memory[0xFFFB], state.d);
    ASSERT_EQ(state.memory[0xFFFA], state.e);
    ASSERT_EQ(state.a, 0x12);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    state.pc = 0;
    state.memory[0] = 0xE5;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, 0xFFF8);
    ASSERT_EQ(state.memory[0xFFF9], state.h);
    ASSERT_EQ(state.memory[0xFFF8], state.l);
    ASSERT_EQ(state.a, 0x12);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    state.pc = 0;
    state.memory[0] = 0xF5;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, 0xFFF6);
    ASSERT_EQ(state.memory[0xFFF7], state.a);
    ASSERT_EQ(state.memory[0xFFF6], state.f);
    ASSERT_EQ(state.a, 0x12);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_POP)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.af = 0x12A1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    // Push data onto stack, then zero out registers.
    state.memory[0] = 0xF5;
    state.memory[1] = 0xE5;
    state.memory[2] = 0xD5;
    state.memory[3] = 0xC5;
    cycles = emulator.ProcessOpCode();
    cycles = emulator.ProcessOpCode();
    cycles = emulator.ProcessOpCode();
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, 0xFFF6);
    state.af = 0;
    state.bc = 0;
    state.de = 0;
    state.hl = 0;
    
    state.pc = 0;
    state.memory[0] = 0xC1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, 0xFFF8);
    ASSERT_EQ(state.af, 0);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, 0);
    ASSERT_EQ(state.hl, 0);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    state.pc = 0;
    state.memory[0] = 0xD1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, 0xFFFA);
    ASSERT_EQ(state.af, 0);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, 0);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    state.pc = 0;
    state.memory[0] = 0xE1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(state.af, 0);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    state.pc = 0;
    state.memory[0] = 0xF1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.af, 0x12A1);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_ADD)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.af = 0x12A1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    // ADD B
    state.a = 1;
    state.bc = 0x0156;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 2);
    ASSERT_EQ(state.bc, 0x0156);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD C
    state.a = 1;
    state.bc = 0x3401;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x81;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 2);
    ASSERT_EQ(state.bc, 0x3401);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD D
    state.a = 1;
    state.bc = BC_VALUE;
    state.de = 0x019A;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x82;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 2);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, 0x019A);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD E
    state.a = 1;
    state.bc = BC_VALUE;
    state.de = 0x7801;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x83;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 2);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, 0x7801);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD H
    state.a = 1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = 0x01DE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x84;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 2);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, 0x01DE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD L
    state.a = 1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = 0xBC01;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x85;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 2);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, 0xBC01);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD (HL)
    state.a = 1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.memory[HL_VALUE] = 1;
    state.pc = 0;
    state.memory[0] = 0x86;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 2);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    // ADD A
    state.a = 1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x87;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 2);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD nn
    state.a = 1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xC6;
    state.memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 2);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    // Test half-carry
    state.a = 0x0F;
    state.bc = 0x0156;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x10);
    ASSERT_EQ(state.bc, 0x0156);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // Test carry
    state.a = 0xF0;
    state.bc = 0x1156;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x01);
    ASSERT_EQ(state.bc, 0x1156);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // Test zero, carry, and half-carry
    state.a = 0xFF;
    state.bc = 0x0156;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x00);
    ASSERT_EQ(state.bc, 0x0156);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_ADC)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.af = 0x12A1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    // ADC B
    state.a = 1;
    state.flags.c = 0;
    state.bc = 0x0156;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x88;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 2);
    ASSERT_EQ(state.bc, 0x0156);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADC B with carry
    state.a = 1;
    state.flags.c = 1;
    state.bc = 0x0156;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x88;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 3);
    ASSERT_EQ(state.bc, 0x0156);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_SUB)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.af = 0x12A1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    // SUB B
    state.a = 1;
    state.bc = 0x0156;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x90;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0);
    ASSERT_EQ(state.bc, 0x0156);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 4);

    state.a = 0x3E;
    state.bc = BC_VALUE;
    state.de = 0x783E;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x93;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, 0x783E);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 4);

    state.a = 0xFF;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x96;
    state.memory[HL_VALUE] = 0x0E;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0xF1);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    state.a = 0x3E;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xD6;
    state.memory[1] = 0x0F;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x2F);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    state.a = 0x3E;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x96;
    state.memory[HL_VALUE] = 0x40;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0xFE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_SBC)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.af = 0x12A1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    state.a = 0x3B;
    state.flags.c = 1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = 0x2ADE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x9C;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x10);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, 0x2ADE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    state.a = 0x3B;
    state.flags.c = 1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xDE;
    state.memory[1] = 0x3A;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x00);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    state.a = 0x3B;
    state.flags.c = 1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x9E;
    state.memory[HL_VALUE] = 0x4F;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0xEB);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_AND)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.af = 0x12A1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    state.a = 0x5A;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = 0xBC3F;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xA5;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x1A);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, 0xBC3F);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    state.a = 0x5A;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xE6;
    state.memory[1] = 0x38;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x18);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    state.a = 0x5A;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xA6;
    state.memory[HL_VALUE] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x00);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_OR)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.af = 0x12A1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    state.a = 0x5A;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xB7;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x5A);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    state.a = 0x5A;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xF6;
    state.memory[1] = 0x03;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x5B);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    state.a = 0x5A;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xB6;
    state.memory[HL_VALUE] = 0x0F;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x5F);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_XOR)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.af = 0x12A1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    state.a = 0xFF;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xAF;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x00);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 4);

    state.a = 0xFF;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xEE;
    state.memory[1] = 0x0F;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0xF0);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    state.a = 0xFF;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xAE;
    state.memory[HL_VALUE] = 0x8A;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x75);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_CP)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.af = 0x12A1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    state.a = 0x3C;
    state.bc = 0x2F56;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xB8;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x3C);
    ASSERT_EQ(state.bc, 0x2F56);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    state.a = 0x3C;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xFE;
    state.memory[1] = 0x3C;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x3C);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    state.a = 0x3C;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0xBE;
    state.memory[HL_VALUE] = 0x40;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x3C);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_INC)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.af = 0x12A1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    state.a = 0xFF;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x3C;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x00);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 4);

    state.a = 0x3C;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x34;
    state.memory[HL_VALUE] = 0x50;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[state.hl], 0x51);
    ASSERT_EQ(state.a, 0x3C);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_DEC)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.af = 0x12A1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    state.a = 0xFF;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = 0xBC01;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x2D;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0xFF);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, 0xBC00);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 4);

    state.a = 0x3C;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x35;
    state.memory[HL_VALUE] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[state.hl], 0xFF);
    ASSERT_EQ(state.a, 0x3C);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_ADD_HL_REG)
{
    Emulator emulator(&state);
    int cycles = 0;
    state.af = 0x1200;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    state.a = 0xFF;
    state.bc = 0x0605;
    state.de = DE_VALUE;
    state.hl = 0x8A23;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x09;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0xFF);
    ASSERT_EQ(state.bc, 0x0605);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, 0x9028);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    state.flags.z = 1;
    state.a = 0x3C;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = 0x8A23;
    state.sp = SP_VALUE;
    state.pc = 0;
    state.memory[0] = 0x29;
    state.memory[HL_VALUE] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x3C);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, 0x1446);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_ADD_SP_N)
{
    Emulator emulator(&state);
    int cycles = 0;

    // ADD SP, N
    ResetState();
    state.memory[0] = 0xE8;
    state.memory[1] = 2;
    state.sp = 0xFFF8;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, 0xFFFA);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    // ADD SP, N
    ResetState();
    state.memory[0] = 0xE8;
    state.memory[1] = 8;
    state.sp = 0xFFF8;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, 0);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0); // Zero isn't set.
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_INC_16)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0x03;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE + 1);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0x13;
    state.de = 0xFFFF;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, 0);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    // No flags should change.
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_DEC_16)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0x2B;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE - 1);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0x3B;
    state.sp = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, 0xFFFF);
    // No flags should change.
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RLCA)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0x07;
    state.a = 0x85;
    state.f = 0xFF;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x0B);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RLA)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0x17;
    state.a = 0x95;
    state.f = 0xFF;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x2B);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RRCA)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0x0F;
    state.a = 0x3B;
    state.f = 0xFF;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x9D);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RRA)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0x1F;
    state.a = 0x81;
    state.f = 0xFF;
    state.flags.c = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x40);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RLC)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x00;
    state.b = 0x85;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.b, 0x0B);
    ASSERT_EQ(state.c, C_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x06;
    state.memory[HL_VALUE] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 0);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RRC)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x09;
    state.c = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.b, B_VALUE);
    ASSERT_EQ(state.c, 0x80);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x0E;
    state.memory[HL_VALUE] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 0);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RL)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x15;
    state.l = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.h, H_VALUE);
    ASSERT_EQ(state.l, 0x00);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x16;
    state.memory[HL_VALUE] = 0x11;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 0x22);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RR)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x1F;
    state.a = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x00);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x1E;
    state.memory[HL_VALUE] = 0x8A;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 0x45);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_SLA)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x22;
    state.d = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.d, 0x00);
    ASSERT_EQ(state.e, E_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x26;
    state.memory[HL_VALUE] = 0xFF;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 0xFE);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_SRA)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x2A;
    state.d = 0x8A;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.d, 0xC5);
    ASSERT_EQ(state.e, E_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x2E;
    state.memory[HL_VALUE] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 0x00);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_SWAP)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x37;
    state.a = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x36;
    state.memory[HL_VALUE] = 0xF0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 0x0F);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_SRL)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x3F;
    state.a = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x3E;
    state.memory[HL_VALUE] = 0xFF;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 0x7F);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_BIT)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x7F;
    state.a = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x80);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x65;
    state.l = 0xEF;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.h, H_VALUE);
    ASSERT_EQ(state.l, 0xEF);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x46;
    state.memory[HL_VALUE] = 0xFE;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 0xFE);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 16);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x4E;
    state.memory[HL_VALUE] = 0xFE;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 0xFE);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RES)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0xBF;
    state.a = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x00);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x8D;
    state.l = 0x3B;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.h, H_VALUE);
    ASSERT_EQ(state.l, 0x39);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0x9E;
    state.memory[HL_VALUE] = 0xFF;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 0xF7);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_SET)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0xDF;
    state.a = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x88);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0xFD;
    state.l = 0x3B;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.h, H_VALUE);
    ASSERT_EQ(state.l, 0xBB);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    state.memory[0] = 0xCB;
    state.memory[1] = 0xDE;
    state.memory[HL_VALUE] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.memory[HL_VALUE], 0x08);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_JP)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xC3;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_JP_NZ)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xC2;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    state.memory[0] = 0xC2;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    state.flags.z = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_JP_Z)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCA;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    state.flags.z = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 16);

    ResetState();
    state.memory[0] = 0xCA;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    state.flags.z = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_JP_NC)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xD2;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    state.memory[0] = 0xD2;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    state.flags.c = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_JP_C)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xDA;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    state.flags.c = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    state.memory[0] = 0xDA;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    state.flags.c = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_JR)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0x18;
    state.memory[1] = 0x05;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0007);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    ResetState();
    state.memory[0xF0] = 0x18;
    state.memory[0xF1] = 0xFB;
    state.pc = 0xF0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x00ED);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_JR_NZ)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0x20;
    state.memory[1] = 0x0F;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0011);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    ResetState();
    state.memory[0] = 0x20;
    state.memory[1] = 0x0F;
    state.flags.z = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_JR_Z)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0x28;
    state.memory[1] = 0x0F;
    state.flags.z = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0011);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 12);

    ResetState();
    state.memory[0] = 0x28;
    state.memory[1] = 0x0F;
    state.flags.z = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_JR_NC)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0x30;
    state.memory[1] = 0x0F;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0011);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    ResetState();
    state.memory[0] = 0x30;
    state.memory[1] = 0x0F;
    state.flags.c = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_JR_C)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0x38;
    state.memory[1] = 0x0F;
    state.flags.c = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0011);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    ResetState();
    state.memory[0] = 0x38;
    state.memory[1] = 0x0F;
    state.flags.c = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_JP_HL)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xE9;
    state.hl = 0x8001;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, 0x8001);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_CALL)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCD;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(state.memory[0xFFFD], 0x00);
    ASSERT_EQ(state.memory[0xFFFC], 0x03);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 24);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_CALL_NZ)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xC4;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(state.memory[0xFFFD], 0x00);
    ASSERT_EQ(state.memory[0xFFFC], 0x03);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 24);

    ResetState();
    state.memory[0] = 0xC4;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    state.flags.z = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_CALL_Z)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xCC;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    state.flags.z = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(state.memory[0xFFFD], 0x00);
    ASSERT_EQ(state.memory[0xFFFC], 0x03);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 24);

    ResetState();
    state.memory[0] = 0xCC;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    state.flags.z = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_CALL_NC)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xD4;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(state.memory[0xFFFD], 0x00);
    ASSERT_EQ(state.memory[0xFFFC], 0x03);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 24);

    ResetState();
    state.memory[0] = 0xD4;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    state.flags.c = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_CALL_C)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xDC;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    state.flags.c = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(state.memory[0xFFFD], 0x00);
    ASSERT_EQ(state.memory[0xFFFC], 0x03);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 24);

    ResetState();
    state.memory[0] = 0xDC;
    state.memory[1] = 0x01;
    state.memory[2] = 0x80;
    state.flags.c = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0003);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RET)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xC9;
    state.memory[0xFFFD] = 0x80;
    state.memory[0xFFFC] = 0x01;
    state.sp = 0xFFFC;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RETI)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xD9;
    state.memory[0xFFFD] = 0x80;
    state.memory[0xFFFC] = 0x01;
    state.sp = 0xFFFC;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RET_NZ)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xC0;
    state.memory[0xFFFD] = 0x80;
    state.memory[0xFFFC] = 0x01;
    state.sp = 0xFFFC;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 20);

    ResetState();
    state.memory[0] = 0xC0;
    state.memory[0xFFFD] = 0x80;
    state.memory[0xFFFC] = 0x01;
    state.sp = 0xFFFC;
    state.flags.z = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RET_Z)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xC8;
    state.memory[0xFFFD] = 0x80;
    state.memory[0xFFFC] = 0x01;
    state.sp = 0xFFFC;
    state.flags.z = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 20);

    ResetState();
    state.memory[0] = 0xC8;
    state.memory[0xFFFD] = 0x80;
    state.memory[0xFFFC] = 0x01;
    state.sp = 0xFFFC;
    state.flags.z = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RET_NC)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xD0;
    state.memory[0xFFFD] = 0x80;
    state.memory[0xFFFC] = 0x01;
    state.sp = 0xFFFC;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 20);

    ResetState();
    state.memory[0] = 0xD0;
    state.memory[0xFFFD] = 0x80;
    state.memory[0xFFFC] = 0x01;
    state.sp = 0xFFFC;
    state.flags.c = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RET_C)
{
    Emulator emulator(&state);
    int cycles = 0;

    ResetState();
    state.memory[0] = 0xD8;
    state.memory[0xFFFD] = 0x80;
    state.memory[0xFFFC] = 0x01;
    state.sp = 0xFFFC;
    state.flags.c = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 20);

    ResetState();
    state.memory[0] = 0xD8;
    state.memory[0xFFFD] = 0x80;
    state.memory[0xFFFC] = 0x01;
    state.sp = 0xFFFC;
    state.flags.c = 0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}