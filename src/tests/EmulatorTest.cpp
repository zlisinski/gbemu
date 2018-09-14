#include "EmulatorTest.h"
#include "../State.h"
#include "../Emulator.h"
#include "../Memory.h"

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


EmulatorTest::EmulatorTest() :
    state(),
    emulator(&state)
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
    state.interrupts->SetEnabled(false);

    state.memory->ClearMemory();
    memory = state.memory->GetBytePtr(0);

    emulator = Emulator(&state);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, TEST_Add8Bit)
{
    uint8_t result;

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
    ASSERT_EQ(result, 0xFFFE);
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

TEST_F(EmulatorTest, TEST_Add16BitSigned)
{
    uint16_t result;

    ResetState();
    result = emulator.Add16BitSigned8Bit(0x00, -1);
    ASSERT_EQ(result, 0xFFFF);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add16BitSigned8Bit(0x00, 0x00);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);

    ResetState();
    result = emulator.Add16BitSigned8Bit(0x0001, 0x0F);
    ASSERT_EQ(result, 0x0010);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.n, 0);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, TEST_Sub8Bit)
{
    uint16_t result;

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
    uint cycles = 0;

    // LD B. B
    ResetState();
    memory[0] = 0x40;
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
    memory[0] = 0x41;
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
    memory[0] = 0x42;
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
    memory[0] = 0x43;
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
    memory[0] = 0x44;
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
    memory[0] = 0x45;
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
    memory[0] = 0x46;
    memory[HL_VALUE] = 0;
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
    memory[0] = 0x47;
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
    memory[0] = 0x70;
    memory[HL_VALUE] = 1;
    state.b = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 1);
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
    uint cycles = 0;

    // LD B. NN
    ResetState();
    memory[0] = 0x06;
    memory[1] = 0x01;
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
    memory[0] = 0x0E;
    memory[1] = 0x01;
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
    memory[0] = 0x16;
    memory[1] = 0x01;
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
    memory[0] = 0x1E;
    memory[1] = 0x01;
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
    memory[0] = 0x26;
    memory[1] = 0x01;
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
    memory[0] = 0x2E;
    memory[1] = 0x01;
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
    memory[0] = 0x36;
    memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 1);
    ASSERT_EQ(state.af, AF_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(cycles, 12);

    // LD A. NN
    ResetState();
    memory[0] = 0x3E;
    memory[1] = 0x01;
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
    uint cycles = 0;

    // LD A, (BC)
    ResetState();
    memory[0] = 0x0A;
    memory[BC_VALUE] = 1;
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
    uint cycles = 0;

    // LD A, (DE)
    ResetState();
    memory[0] = 0x1A;
    memory[DE_VALUE] = 1;
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
    uint cycles = 0;

    // LD A, (0xFF00+C)
    ResetState();
    memory[0] = 0xF2;
    memory[0xFF00 + C_VALUE] = 1;
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
    uint cycles = 0;

    // LD (0xFF00+C), A
    ResetState();
    memory[0] = 0xE2;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[0xFF00 + C_VALUE], 1);
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
    uint cycles = 0;

    // LD A, (0xFF00+N)
    ResetState();
    memory[0] = 0xF0;
    memory[1] = 0x02;
    memory[0xFF02] = 1;
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
    uint cycles = 0;

    // LD (0xFF00+N), A
    ResetState();
    memory[0] = 0xE0;
    memory[1] = 0x03;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[0xFF03], 1);
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
    uint cycles = 0;

    // LD A, (NN)
    ResetState();
    memory[0] = 0xFA;
    memory[1] = 0x02;
    memory[2] = 0x01;
    memory[0x0102] = 1;
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
    uint cycles = 0;

    // LD (NN), A
    ResetState();
    memory[0] = 0xEA;
    memory[1] = 0x02;
    memory[2] = 0x01;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[0x0102], 1);
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
    uint cycles = 0;

    // LD A, (HL+)
    ResetState();
    memory[0] = 0x2A;
    memory[HL_VALUE] = 1;
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
    uint cycles = 0;

    // LD A, (HL-)
    ResetState();
    memory[0] = 0x3A;
    memory[HL_VALUE] = 1;
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
    uint cycles = 0;

    // LD (BC), A
    ResetState();
    memory[0] = 0x02;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[BC_VALUE], 1);
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
    uint cycles = 0;

    // LD (DE), A
    ResetState();
    memory[0] = 0x12;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[DE_VALUE], 1);
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
    uint cycles = 0;

    // LD (HL+), A
    ResetState();
    memory[0] = 0x22;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 1);
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
    uint cycles = 0;

    // LD (HL-), A
    ResetState();
    memory[0] = 0x32;
    state.a = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 1);
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
    uint cycles = 0;

    // LD BC. NN
    ResetState();
    memory[0] = 0x01;
    memory[1] = 0x01;
    memory[2] = 0x02;
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
    memory[0] = 0x11;
    memory[1] = 0x01;
    memory[2] = 0x02;
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
    memory[0] = 0x21;
    memory[1] = 0x01;
    memory[2] = 0x02;
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
    memory[0] = 0x31;
    memory[1] = 0x01;
    memory[2] = 0x02;
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
    uint cycles = 0;

    // LD SP, HL
    ResetState();
    memory[0] = 0xF9;
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
    uint cycles = 0;

    ResetState();
    state.sp = 0;
    memory[0] = 0xF8;
    memory[1] = 0x0F;
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

    // Half-carry is on bit 3->4
    ResetState();
    state.sp = 0x000F;
    memory[0] = 0xF8;
    memory[1] = 0x0F;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.hl, 0x001E);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.sp, 0x000F);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    // Half-carry is on bit 3->4
    ResetState();
    state.sp = 0x0FFF;
    memory[0] = 0xF8;
    memory[1] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.hl, 0x1000);
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.sp, 0x0FFF);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    // Test carry and half-carry
    ResetState();
    state.sp = 0xFFFF;
    memory[0] = 0xF8;
    memory[1] = 0x01;
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
    uint cycles = 0;

    memory[0] = 0x08;
    memory[1] = 0x00;
    memory[2] = 0xC1;
    state.sp = 0xFFF8;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[0xC100], 0xF8);
    ASSERT_EQ(memory[0xC101], 0xFF);
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
    uint cycles = 0;
    state.a = 0x12;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    memory[0] = 0xC5;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], state.b);
    ASSERT_EQ(memory[0xFFFC], state.c);
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
    memory[0] = 0xD5;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, 0xFFFA);
    ASSERT_EQ(memory[0xFFFB], state.d);
    ASSERT_EQ(memory[0xFFFA], state.e);
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
    memory[0] = 0xE5;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, 0xFFF8);
    ASSERT_EQ(memory[0xFFF9], state.h);
    ASSERT_EQ(memory[0xFFF8], state.l);
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
    memory[0] = 0xF5;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, 0xFFF6);
    ASSERT_EQ(memory[0xFFF7], state.a);
    ASSERT_EQ(memory[0xFFF6], state.f);
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
    uint cycles = 0;
    state.af = 0x12A1;
    state.bc = BC_VALUE;
    state.de = DE_VALUE;
    state.hl = HL_VALUE;
    state.sp = SP_VALUE;

    // Push data onto stack, then zero out registers.
    memory[0] = 0xF5;
    memory[1] = 0xE5;
    memory[2] = 0xD5;
    memory[3] = 0xC5;
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
    memory[0] = 0xC1;
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
    memory[0] = 0xD1;
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
    memory[0] = 0xE1;
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
    memory[0] = 0xF1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.af, 0x12A0);
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
    uint cycles = 0;
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
    memory[0] = 0x80;
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
    memory[0] = 0x81;
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
    memory[0] = 0x82;
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
    memory[0] = 0x83;
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
    memory[0] = 0x84;
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
    memory[0] = 0x85;
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
    memory[HL_VALUE] = 1;
    state.pc = 0;
    memory[0] = 0x86;
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
    memory[0] = 0x87;
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
    memory[0] = 0xC6;
    memory[1] = 0x01;
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
    memory[0] = 0x80;
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
    memory[0] = 0x80;
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
    memory[0] = 0x80;
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
    uint cycles = 0;
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
    memory[0] = 0x88;
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
    memory[0] = 0x88;
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
    uint cycles = 0;
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
    memory[0] = 0x90;
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
    memory[0] = 0x93;
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
    memory[0] = 0x96;
    memory[HL_VALUE] = 0x0E;
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
    memory[0] = 0xD6;
    memory[1] = 0x0F;
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
    memory[0] = 0x96;
    memory[HL_VALUE] = 0x40;
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
    uint cycles = 0;
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
    memory[0] = 0x9C;
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
    memory[0] = 0xDE;
    memory[1] = 0x3A;
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
    memory[0] = 0x9E;
    memory[HL_VALUE] = 0x4F;
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
    uint cycles = 0;
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
    memory[0] = 0xA5;
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
    memory[0] = 0xE6;
    memory[1] = 0x38;
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
    memory[0] = 0xA6;
    memory[HL_VALUE] = 0x00;
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
    uint cycles = 0;
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
    memory[0] = 0xB7;
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
    memory[0] = 0xF6;
    memory[1] = 0x03;
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
    memory[0] = 0xB6;
    memory[HL_VALUE] = 0x0F;
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
    uint cycles = 0;
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
    memory[0] = 0xAF;
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
    memory[0] = 0xEE;
    memory[1] = 0x0F;
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
    memory[0] = 0xAE;
    memory[HL_VALUE] = 0x8A;
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
    uint cycles = 0;
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
    memory[0] = 0xB8;
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
    memory[0] = 0xFE;
    memory[1] = 0x3C;
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
    memory[0] = 0xBE;
    memory[HL_VALUE] = 0x40;
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
    uint cycles = 0;
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
    memory[0] = 0x3C;
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
    memory[0] = 0x34;
    memory[HL_VALUE] = 0x50;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[state.hl], 0x51);
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
    uint cycles = 0;
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
    memory[0] = 0x2D;
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
    memory[0] = 0x35;
    memory[HL_VALUE] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[state.hl], 0xFF);
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
    uint cycles = 0;
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
    memory[0] = 0x09;
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
    memory[0] = 0x29;
    memory[HL_VALUE] = 0x00;
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
    uint cycles = 0;

    // ADD SP, N
    ResetState();
    memory[0] = 0xE8;
    memory[1] = 2;
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
    memory[0] = 0xE8;
    memory[1] = 8;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0x03;
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
    memory[0] = 0x13;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0x2B;
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
    memory[0] = 0x3B;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0x07;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0x17;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0x0F;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0x1F;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x00;
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
    memory[0] = 0xCB;
    memory[1] = 0x06;
    memory[HL_VALUE] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 0);
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x09;
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
    memory[0] = 0xCB;
    memory[1] = 0x0E;
    memory[HL_VALUE] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 0);
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x15;
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
    memory[0] = 0xCB;
    memory[1] = 0x16;
    memory[HL_VALUE] = 0x11;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 0x22);
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x1F;
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
    memory[0] = 0xCB;
    memory[1] = 0x1E;
    memory[HL_VALUE] = 0x8A;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 0x45);
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x22;
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
    memory[0] = 0xCB;
    memory[1] = 0x26;
    memory[HL_VALUE] = 0xFF;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 0xFE);
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x2A;
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
    memory[0] = 0xCB;
    memory[1] = 0x2E;
    memory[HL_VALUE] = 0x01;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 0x00);
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x37;
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
    memory[0] = 0xCB;
    memory[1] = 0x36;
    memory[HL_VALUE] = 0xF0;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 0x0F);
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x3F;
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
    memory[0] = 0xCB;
    memory[1] = 0x3E;
    memory[HL_VALUE] = 0xFF;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 0x7F);
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x7F;
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
    memory[0] = 0xCB;
    memory[1] = 0x65;
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
    memory[0] = 0xCB;
    memory[1] = 0x46;
    memory[HL_VALUE] = 0xFE;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 0xFE);
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
    ASSERT_EQ(cycles, 12);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x4E;
    memory[HL_VALUE] = 0xFE;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 0xFE);
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
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RES)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0xBF;
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
    memory[0] = 0xCB;
    memory[1] = 0x8D;
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
    memory[0] = 0xCB;
    memory[1] = 0x9E;
    memory[HL_VALUE] = 0xFF;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 0xF7);
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0xDF;
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
    memory[0] = 0xCB;
    memory[1] = 0xFD;
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
    memory[0] = 0xCB;
    memory[1] = 0xDE;
    memory[HL_VALUE] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(memory[HL_VALUE], 0x08);
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xC3;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xC2;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    memory[0] = 0xC2;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCA;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    memory[0] = 0xCA;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xD2;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    memory[0] = 0xD2;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xDA;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    memory[0] = 0xDA;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0x18;
    memory[1] = 0x05;
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
    memory[0xF0] = 0x18;
    memory[0xF1] = 0xFB;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0x20;
    memory[1] = 0x0F;
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
    memory[0] = 0x20;
    memory[1] = 0x0F;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0x28;
    memory[1] = 0x0F;
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
    memory[0] = 0x28;
    memory[1] = 0x0F;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0x30;
    memory[1] = 0x0F;
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
    memory[0] = 0x30;
    memory[1] = 0x0F;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0x38;
    memory[1] = 0x0F;
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
    memory[0] = 0x38;
    memory[1] = 0x0F;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xE9;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCD;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x00);
    ASSERT_EQ(memory[0xFFFC], 0x03);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 24);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_CALL_NZ)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xC4;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x00);
    ASSERT_EQ(memory[0xFFFC], 0x03);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 24);

    ResetState();
    memory[0] = 0xC4;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCC;
    memory[1] = 0x01;
    memory[2] = 0x80;
    state.flags.z = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x00);
    ASSERT_EQ(memory[0xFFFC], 0x03);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 1);
    ASSERT_EQ(cycles, 24);

    ResetState();
    memory[0] = 0xCC;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xD4;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x00);
    ASSERT_EQ(memory[0xFFFC], 0x03);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 24);

    ResetState();
    memory[0] = 0xD4;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xDC;
    memory[1] = 0x01;
    memory[2] = 0x80;
    state.flags.c = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x8001);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x00);
    ASSERT_EQ(memory[0xFFFC], 0x03);
    ASSERT_EQ(state.flags.c, 1);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 24);

    ResetState();
    memory[0] = 0xDC;
    memory[1] = 0x01;
    memory[2] = 0x80;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xC9;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xD9;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
    state.sp = 0xFFFC;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.interrupts->Enabled(), true);
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xC0;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
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
    memory[0] = 0xC0;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xC8;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
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
    memory[0] = 0xC8;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xD0;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
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
    memory[0] = 0xD0;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
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
    uint cycles = 0;

    ResetState();
    memory[0] = 0xD8;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
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
    memory[0] = 0xD8;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
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

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_RST)
{
    uint cycles = 0;

    ResetState();
    memory[0x1234] = 0xC7;
    state.pc = 0x1234;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0000);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xCF;
    state.pc = 0x1234;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0008);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xD7;
    state.pc = 0x1234;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0010);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xDF;
    state.pc = 0x1234;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0018);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xE7;
    state.pc = 0x1234;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0020);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xEF;
    state.pc = 0x1234;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0028);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xF7;
    state.pc = 0x1234;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0030);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xFF;
    state.pc = 0x1234;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0038);
    ASSERT_EQ(state.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_CPL)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x2F;
    state.a = 0x35;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0xCA);
    ASSERT_EQ(state.bc, BC_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 1);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_NOP)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
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
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_SCF)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x37;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
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

    ResetState();
    memory[0] = 0x37;
    state.flags.c = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
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

TEST_F(EmulatorTest, Test_CCF)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x3F;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
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

    ResetState();
    memory[0] = 0x3F;
    state.flags.c = 1;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, A_VALUE);
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
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_DAA)
{
    uint cycles = 0;

    ResetState();
    state.a = 0x45;
    state.b = 0x38;
    memory[0] = 0x80; // ADD A, B
    memory[1] = 0x27; // DAA
    cycles = emulator.ProcessOpCode();
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x83);
    ASSERT_EQ(state.b, 0x38);
    ASSERT_EQ(state.c, C_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 0);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    ResetState();
    state.a = 0x83;
    state.b = 0x38;
    memory[0] = 0x90; // SUB A, B
    memory[1] = 0x27; // DAA
    cycles = emulator.ProcessOpCode();
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.a, 0x45);
    ASSERT_EQ(state.b, 0x38);
    ASSERT_EQ(state.c, C_VALUE);
    ASSERT_EQ(state.de, DE_VALUE);
    ASSERT_EQ(state.hl, HL_VALUE);
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(state.flags.c, 0);
    ASSERT_EQ(state.flags.h, 0);
    ASSERT_EQ(state.flags.n, 1);
    ASSERT_EQ(state.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_DI)
{
    uint cycles = 0;

    ResetState();
    state.interrupts->SetEnabled(true);
    memory[0] = 0xF3;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.interrupts->Enabled(), false);
    ASSERT_EQ(state.a, A_VALUE);
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

    ResetState();
    state.interrupts->SetEnabled(false);
    memory[0] = 0xF3;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.interrupts->Enabled(), false);
    ASSERT_EQ(state.a, A_VALUE);
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
}


///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_EI)
{
    uint cycles = 0;

    ResetState();
    state.interrupts->SetEnabled(false);
    memory[0] = 0xFB;
    memory[1] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.interrupts->Enabled(), false); // EI has a delayed effect.
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.interrupts->Enabled(), true);
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
    ASSERT_EQ(cycles, 4);

    ResetState();
    state.interrupts->SetEnabled(true);
    memory[0] = 0xFB;
    memory[1] = 0x00;
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.interrupts->Enabled(), true); // EI has a delayed effect, but it was already enabled.
    cycles = emulator.ProcessOpCode();
    ASSERT_EQ(state.interrupts->Enabled(), true);
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
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(EmulatorTest, Test_Interrupts)
{
    uint cycles = 0;

    ResetState();
    state.interrupts->SetEnabled(true);
    memory[eRegIE] = 0x01;
    memory[eRegIF] = 0x01;
    memory[0] = 0x00;
    memory[1] = 0x00;
    memory[0x40] = 0x3C; // INC A
    memory[0x41] = 0xC9; // RET
    cycles = emulator.ProcessOpCode(); // Process interrupt
    ASSERT_EQ(state.pc, 0x0040);
    ASSERT_EQ(state.sp, SP_VALUE - 2);
    ASSERT_EQ(cycles, 20);
    
    cycles = emulator.ProcessOpCode(); // Process INC A
    ASSERT_EQ(state.pc, 0x0041);
    ASSERT_EQ(state.a, A_VALUE + 1);
    ASSERT_EQ(cycles, 4);

    cycles = emulator.ProcessOpCode(); // Process RET
    ASSERT_EQ(state.pc, 0x0000);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(cycles, 16);


    ResetState();
    state.interrupts->SetEnabled(true);
    memory[eRegIE] = 0x00;
    memory[eRegIF] = 0x01;
    memory[0] = 0x00;
    memory[1] = 0x00;
    memory[0x40] = 0x3C; // INC A
    memory[0x41] = 0xC9; // RET
    cycles = emulator.ProcessOpCode(); // IE disabled
    ASSERT_EQ(state.pc, 0x0001);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(cycles, 4);
    
    cycles = emulator.ProcessOpCode(); // IE disabled
    ASSERT_EQ(state.pc, 0x0002);
    ASSERT_EQ(state.sp, SP_VALUE);
    ASSERT_EQ(cycles, 4);


    ResetState();
    state.interrupts->SetEnabled(true);
    memory[eRegIE] = 0x02;
    memory[eRegIF] = 0x02;
    cycles = emulator.ProcessOpCode(); // Process interrupt
    ASSERT_EQ(state.pc, 0x0048);
    ASSERT_EQ(state.sp, SP_VALUE - 2);
    ASSERT_EQ(cycles, 20);

    ResetState();
    state.interrupts->SetEnabled(true);
    memory[eRegIE] = 0x04;
    memory[eRegIF] = 0x04;
    cycles = emulator.ProcessOpCode(); // Process interrupt
    ASSERT_EQ(state.pc, 0x0050);
    ASSERT_EQ(state.sp, SP_VALUE - 2);
    ASSERT_EQ(cycles, 20);

    ResetState();
    state.interrupts->SetEnabled(true);
    memory[eRegIE] = 0x08;
    memory[eRegIF] = 0x08;
    cycles = emulator.ProcessOpCode(); // Process interrupt
    ASSERT_EQ(state.pc, 0x0058);
    ASSERT_EQ(state.sp, SP_VALUE - 2);
    ASSERT_EQ(cycles, 20);

    ResetState();
    state.interrupts->SetEnabled(true);
    memory[eRegIE] = 0x10;
    memory[eRegIF] = 0x10;
    cycles = emulator.ProcessOpCode(); // Process interrupt
    ASSERT_EQ(state.pc, 0x0060);
    ASSERT_EQ(state.sp, SP_VALUE - 2);
    ASSERT_EQ(cycles, 20);
}
