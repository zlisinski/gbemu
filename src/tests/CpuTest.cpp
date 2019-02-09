#include "main.h"
#include "CpuTest.h"
#include "../Cpu.h"
#include "../Interrupt.h"
#include "../Memory.h"
#include "../Timer.h"

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


CpuTest::CpuTest()
{
    memory_ = new Memory();
    interrupts = new Interrupt(memory_->GetBytePtr(eRegIE), memory_->GetBytePtr(eRegIF));
    timer = new Timer(memory_->GetBytePtr(eRegTIMA), memory_->GetBytePtr(eRegTMA),
                      memory_->GetBytePtr(eRegTAC), memory_->GetBytePtr(eRegDIV), interrupts);
    cpu = new Cpu(interrupts, memory_, timer);
}

CpuTest::~CpuTest()
{
    delete cpu;
    delete timer;
    delete interrupts;
    delete memory_;
}

void CpuTest::SetUp()
{
    ResetState();
}

void CpuTest::TearDown()
{

}

void CpuTest::ResetState()
{
    cpu->reg.af = AF_VALUE;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;

    interrupts->SetEnabled(false);

    memory_->ClearMemory();
    memory = memory_->GetBytePtr(0);

    timer->WriteDIV();
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, TEST_Add8Bit)
{
    uint8_t result;

    result = cpu->Add8Bit(0x00, 0x00);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add8Bit(0x00, 0x01);
    ASSERT_EQ(result, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add8Bit(0x01, 0x01);
    ASSERT_EQ(result, 2);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add8Bit(0x01, 0x01, 1);
    ASSERT_EQ(result, 3);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add8Bit(0x0F, 0x01);
    ASSERT_EQ(result, 0x10);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add8Bit(0x0E, 0x01, 1);
    ASSERT_EQ(result, 0x10);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add8Bit(0x0F, 0x0F);
    ASSERT_EQ(result, 0x1E);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add8Bit(0xF1, 0xFE);
    ASSERT_EQ(result, 0xEF);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add8Bit(0xFF, 0xFF);
    ASSERT_EQ(result, 0xFE);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add8Bit(0xA5, 0x5A);
    ASSERT_EQ(result, 0xFF);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add8Bit(0xA5, 0x5A, 1);
    ASSERT_EQ(result, 0x00);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add8Bit(0xFF, 0x01);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, TEST_Add16Bit)
{
    uint16_t result;

    result = cpu->Add16Bit(0x00, 0x00);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add16Bit(0x00, 0x01);
    ASSERT_EQ(result, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add16Bit(0x01, 0x01);
    ASSERT_EQ(result, 2);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add16Bit(0x0F00, 0x0100);
    ASSERT_EQ(result, 0x1000);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add16Bit(0x0F00, 0x0F00);
    ASSERT_EQ(result, 0x1E00);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add16Bit(0xF100, 0xFE00);
    ASSERT_EQ(result, 0xEF00);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add16Bit(0xFFFF, 0xFFFF);
    ASSERT_EQ(result, 0xFFFE);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add16Bit(0xA5A5, 0x5A5A);
    ASSERT_EQ(result, 0xFFFF);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add16Bit(0xFFFF, 0x0001);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, TEST_Add16BitSigned)
{
    uint16_t result;

    ResetState();
    result = cpu->Add16BitSigned8Bit(0x00, -1);
    ASSERT_EQ(result, 0xFFFF);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add16BitSigned8Bit(0x00, 0x00);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);

    ResetState();
    result = cpu->Add16BitSigned8Bit(0x0001, 0x0F);
    ASSERT_EQ(result, 0x0010);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, TEST_Sub8Bit)
{
    uint16_t result;

    result = cpu->Sub8Bit(0x00, 0x00);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);

    ResetState();
    result = cpu->Sub8Bit(0x01, 0x00);
    ASSERT_EQ(result, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);

    ResetState();
    result = cpu->Sub8Bit(0x01, 0x00, 1);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);

    ResetState();
    result = cpu->Sub8Bit(0x11, 0x02);
    ASSERT_EQ(result, 0x0F);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);

    ResetState();
    result = cpu->Sub8Bit(0x11, 0x01);
    ASSERT_EQ(result, 0x10);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);

    ResetState();
    result = cpu->Sub8Bit(0x11, 0x01, 1);
    ASSERT_EQ(result, 0x0F);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);

    ResetState();
    result = cpu->Sub8Bit(0xA5, 0x5A);
    ASSERT_EQ(result, 0x4B);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);

    ResetState();
    result = cpu->Sub8Bit(0x01, 0xFF);
    ASSERT_EQ(result, 0x02);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.n, 1);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_REG_REG_8Bit)
{
    uint16_t cycles = 0;

    // LD B. B
    ResetState();
    memory[0] = 0x40;
    cpu->reg.b = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.b, 0);
    ASSERT_EQ(cpu->reg.c, C_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // LD B. C
    ResetState();
    memory[0] = 0x41;
    cpu->reg.c = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.b, 0);
    ASSERT_EQ(cpu->reg.c, 0);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // LD B. D
    ResetState();
    memory[0] = 0x42;
    cpu->reg.d = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.b, 0);
    ASSERT_EQ(cpu->reg.c, C_VALUE);
    ASSERT_EQ(cpu->reg.d, 0);
    ASSERT_EQ(cpu->reg.e, E_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // LD B. E
    ResetState();
    memory[0] = 0x43;
    cpu->reg.e = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.b, 0);
    ASSERT_EQ(cpu->reg.c, C_VALUE);
    ASSERT_EQ(cpu->reg.d, D_VALUE);
    ASSERT_EQ(cpu->reg.e, 0);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // LD B. H
    ResetState();
    memory[0] = 0x44;
    cpu->reg.h = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.b, 0);
    ASSERT_EQ(cpu->reg.c, C_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.h, 0);
    ASSERT_EQ(cpu->reg.l, L_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // LD B. L
    ResetState();
    memory[0] = 0x45;
    cpu->reg.l = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.b, 0);
    ASSERT_EQ(cpu->reg.c, C_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.h, H_VALUE);
    ASSERT_EQ(cpu->reg.l, 0);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // LD B. (HL)
    ResetState();
    memory[0] = 0x46;
    memory[HL_VALUE] = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.b, 0);
    ASSERT_EQ(cpu->reg.c, C_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);

    // LD B. A
    ResetState();
    memory[0] = 0x47;
    cpu->reg.a = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.b, 0);
    ASSERT_EQ(cpu->reg.c, C_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 4);

    // TODO: Add the other 56 opcodes

    // LD (HL), B
    ResetState();
    memory[0] = 0x70;
    memory[HL_VALUE] = 1;
    cpu->reg.b = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 1);
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.b, 1);
    ASSERT_EQ(cpu->reg.c, C_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_REG_NN_8Bit)
{
    uint cycles = 0;

    // LD B. NN
    ResetState();
    memory[0] = 0x06;
    memory[1] = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.b, 1);
    ASSERT_EQ(cpu->reg.c, C_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cycles, 8);

    // LD C. NN
    ResetState();
    memory[0] = 0x0E;
    memory[1] = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.b, B_VALUE);
    ASSERT_EQ(cpu->reg.c, 1);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cycles, 8);

    // LD D. NN
    ResetState();
    memory[0] = 0x16;
    memory[1] = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.d, 1);
    ASSERT_EQ(cpu->reg.e, E_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cycles, 8);

    // LD E. NN
    ResetState();
    memory[0] = 0x1E;
    memory[1] = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.d, D_VALUE);
    ASSERT_EQ(cpu->reg.e, 1);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cycles, 8);

    // LD H. NN
    ResetState();
    memory[0] = 0x26;
    memory[1] = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.h, 1);
    ASSERT_EQ(cpu->reg.l, L_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cycles, 8);

    // LD L. NN
    ResetState();
    memory[0] = 0x2E;
    memory[1] = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.h, H_VALUE);
    ASSERT_EQ(cpu->reg.l, 1);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cycles, 8);

    // LD (HL). NN
    ResetState();
    memory[0] = 0x36;
    memory[1] = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 1);
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cycles, 12);

    // LD A. NN
    ResetState();
    memory[0] = 0x3E;
    memory[1] = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_A_BC)
{
    uint cycles = 0;

    // LD A, (BC)
    ResetState();
    memory[0] = 0x0A;
    memory[BC_VALUE] = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_A_DE)
{
    uint cycles = 0;

    // LD A, (DE)
    ResetState();
    memory[0] = 0x1A;
    memory[DE_VALUE] = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_A_0xFF00_PLUS_C)
{
    uint cycles = 0;

    // LD A, (0xFF00+C)
    ResetState();
    memory[0] = 0xF2;
    memory[0xFF00 + C_VALUE] = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_0xFF00_PLUS_C_A)
{
    uint cycles = 0;

    // LD (0xFF00+C), A
    ResetState();
    memory[0] = 0xE2;
    cpu->reg.a = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[0xFF00 + C_VALUE], 1);
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_A_0xFF00_PLUS_N)
{
    uint cycles = 0;

    // LD A, (0xFF00+N)
    ResetState();
    memory[0] = 0xF0;
    memory[1] = 0xFE;
    memory[0xFFFE] = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_0xFF00_PLUS_N_A)
{
    uint cycles = 0;

    // LD (0xFF00+N), A
    ResetState();
    memory[0] = 0xE0;
    memory[1] = 0x03;
    cpu->reg.a = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[0xFF03], 1);
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_A_NN)
{
    uint cycles = 0;

    // LD A, (NN)
    ResetState();
    memory[0] = 0xFA;
    memory[1] = 0x02;
    memory[2] = 0x01;
    memory[0x0102] = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_NN_A)
{
    uint cycles = 0;

    // LD (NN), A
    ResetState();
    memory[0] = 0xEA;
    memory[1] = 0x80;
    memory[2] = 0xFF;
    cpu->reg.a = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[0xFF80], 1);
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_A_HL_PLUS)
{
    uint cycles = 0;

    // LD A, (HL+)
    ResetState();
    memory[0] = 0x2A;
    memory[HL_VALUE] = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE + 1);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_A_HL_MINUS)
{
    uint cycles = 0;

    // LD A, (HL-)
    ResetState();
    memory[0] = 0x3A;
    memory[HL_VALUE] = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE - 1);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_BC_A)
{
    uint cycles = 0;

    // LD (BC), A
    ResetState();
    cpu->reg.bc = 0xFF80;
    memory[0] = 0x02;
    cpu->reg.a = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[0xFF80], 1);
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, 0xFF80);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_DE_A)
{
    uint cycles = 0;

    // LD (DE), A
    ResetState();
    cpu->reg.de = 0xFF80;
    memory[0] = 0x12;
    cpu->reg.a = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[0xFF80], 1);
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, 0xFF80);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_HLI_A)
{
    uint cycles = 0;

    // LD (HL+), A
    ResetState();
    memory[0] = 0x22;
    cpu->reg.a = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 1);
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE + 1);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_HLD_A)
{
    uint cycles = 0;

    // LD (HL-), A
    ResetState();
    memory[0] = 0x32;
    cpu->reg.a = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 1);
    ASSERT_EQ(cpu->reg.a, 1);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE - 1);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_REG_NN_16Bit)
{
    uint cycles = 0;

    // LD BC. NN
    ResetState();
    memory[0] = 0x01;
    memory[1] = 0x01;
    memory[2] = 0x02;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.bc, 0x0201);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cycles, 12);

    // LD DE. NN
    ResetState();
    memory[0] = 0x11;
    memory[1] = 0x01;
    memory[2] = 0x02;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, 0x0201);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cycles, 12);

    // LD HL. NN
    ResetState();
    memory[0] = 0x21;
    memory[1] = 0x01;
    memory[2] = 0x02;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, 0x0201);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cycles, 12);

    // LD SP. NN
    ResetState();
    memory[0] = 0x31;
    memory[1] = 0x01;
    memory[2] = 0x02;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.af, AF_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, 0x0201);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_SP_HL)
{
    uint cycles = 0;

    // LD SP, HL
    ResetState();
    memory[0] = 0xF9;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.f, F_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_HL_SP_PLUS_NN)
{
    uint cycles = 0;

    ResetState();
    cpu->reg.sp = 0;
    memory[0] = 0xF8;
    memory[1] = 0x0F;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.hl, 0x000F);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.sp, 0);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    // Half-carry is on bit 3->4
    ResetState();
    cpu->reg.sp = 0x000F;
    memory[0] = 0xF8;
    memory[1] = 0x0F;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.hl, 0x001E);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.sp, 0x000F);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    // Half-carry is on bit 3->4
    ResetState();
    cpu->reg.sp = 0x0FFF;
    memory[0] = 0xF8;
    memory[1] = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.hl, 0x1000);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.sp, 0x0FFF);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    // Test carry and half-carry
    ResetState();
    cpu->reg.sp = 0xFFFF;
    memory[0] = 0xF8;
    memory[1] = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.hl, 0x0000);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.sp, 0xFFFF);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_LD_NN_SP)
{
    uint cycles = 0;

    memory[0] = 0x08;
    memory[1] = 0x00;
    memory[2] = 0xC1;
    cpu->reg.sp = 0xFFF8;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[0xC100], 0xF8);
    ASSERT_EQ(memory[0xC101], 0xFF);
    ASSERT_EQ(cpu->reg.a, 0x12);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, 0xFFF8);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 20);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_PUSH)
{
    uint cycles = 0;
    cpu->reg.a = 0x12;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    memory[0] = 0xC5;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], cpu->reg.b);
    ASSERT_EQ(memory[0xFFFC], cpu->reg.c);
    ASSERT_EQ(cpu->reg.a, 0x12);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    cpu->reg.pc = 0;
    memory[0] = 0xD5;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.sp, 0xFFFA);
    ASSERT_EQ(memory[0xFFFB], cpu->reg.d);
    ASSERT_EQ(memory[0xFFFA], cpu->reg.e);
    ASSERT_EQ(cpu->reg.a, 0x12);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 32);

    cpu->reg.pc = 0;
    memory[0] = 0xE5;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.sp, 0xFFF8);
    ASSERT_EQ(memory[0xFFF9], cpu->reg.h);
    ASSERT_EQ(memory[0xFFF8], cpu->reg.l);
    ASSERT_EQ(cpu->reg.a, 0x12);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 48);

    cpu->reg.pc = 0;
    memory[0] = 0xF5;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.sp, 0xFFF6);
    ASSERT_EQ(memory[0xFFF7], cpu->reg.a);
    ASSERT_EQ(memory[0xFFF6], cpu->reg.f);
    ASSERT_EQ(cpu->reg.a, 0x12);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 64);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_POP)
{
    uint cycles = 0;
    cpu->reg.af = 0x12A1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    // Push data onto stack, then zero out registers.
    memory[0] = 0xF5;
    memory[1] = 0xE5;
    memory[2] = 0xD5;
    memory[3] = 0xC5;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.sp, 0xFFF6);
    cpu->reg.af = 0;
    cpu->reg.bc = 0;
    cpu->reg.de = 0;
    cpu->reg.hl = 0;
    
    cpu->reg.pc = 0;
    memory[0] = 0xC1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.sp, 0xFFF8);
    ASSERT_EQ(cpu->reg.af, 0);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, 0);
    ASSERT_EQ(cpu->reg.hl, 0);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 76);

    cpu->reg.pc = 0;
    memory[0] = 0xD1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.sp, 0xFFFA);
    ASSERT_EQ(cpu->reg.af, 0);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, 0);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 88);

    cpu->reg.pc = 0;
    memory[0] = 0xE1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(cpu->reg.af, 0);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 100);

    cpu->reg.pc = 0;
    memory[0] = 0xF1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.af, 0x12A0);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 112);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_ADD)
{
    uint cycles = 0;
    cpu->reg.af = 0x12A1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    // ADD B
    ResetState();
    cpu->reg.a = 1;
    cpu->reg.bc = 0x0156;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 2);
    ASSERT_EQ(cpu->reg.bc, 0x0156);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD C
    ResetState();
    cpu->reg.a = 1;
    cpu->reg.bc = 0x3401;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x81;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 2);
    ASSERT_EQ(cpu->reg.bc, 0x3401);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD D
    ResetState();
    cpu->reg.a = 1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = 0x019A;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x82;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 2);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, 0x019A);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD E
    ResetState();
    cpu->reg.a = 1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = 0x7801;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x83;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 2);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, 0x7801);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD H
    ResetState();
    cpu->reg.a = 1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = 0x01DE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x84;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 2);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, 0x01DE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD L
    ResetState();
    cpu->reg.a = 1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = 0xBC01;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x85;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 2);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, 0xBC01);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD (HL)
    ResetState();
    cpu->reg.a = 1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    memory[HL_VALUE] = 1;
    cpu->reg.pc = 0;
    memory[0] = 0x86;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 2);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    // ADD A
    ResetState();
    cpu->reg.a = 1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x87;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 2);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADD nn
    ResetState();
    cpu->reg.a = 1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xC6;
    memory[1] = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 2);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    // Test half-carry
    ResetState();
    cpu->reg.a = 0x0F;
    cpu->reg.bc = 0x0156;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x10);
    ASSERT_EQ(cpu->reg.bc, 0x0156);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // Test carry
    ResetState();
    cpu->reg.a = 0xF0;
    cpu->reg.bc = 0x1156;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x01);
    ASSERT_EQ(cpu->reg.bc, 0x1156);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // Test zero, carry, and half-carry
    ResetState();
    cpu->reg.a = 0xFF;
    cpu->reg.bc = 0x0156;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x00);
    ASSERT_EQ(cpu->reg.bc, 0x0156);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_ADC)
{
    uint cycles = 0;
    cpu->reg.af = 0x12A1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    // ADC B
    ResetState();
    cpu->reg.a = 1;
    cpu->reg.flags.c = 0;
    cpu->reg.bc = 0x0156;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x88;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 2);
    ASSERT_EQ(cpu->reg.bc, 0x0156);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    // ADC B with carry
    ResetState();
    cpu->reg.a = 1;
    cpu->reg.flags.c = 1;
    cpu->reg.bc = 0x0156;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x88;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 3);
    ASSERT_EQ(cpu->reg.bc, 0x0156);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_SUB)
{
    uint cycles = 0;
    cpu->reg.af = 0x12A1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    // SUB B
    ResetState();
    cpu->reg.a = 1;
    cpu->reg.bc = 0x0156;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x90;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0);
    ASSERT_EQ(cpu->reg.bc, 0x0156);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 4);

    ResetState();
    cpu->reg.a = 0x3E;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = 0x783E;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x93;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, 0x783E);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 4);

    ResetState();
    cpu->reg.a = 0xFF;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x96;
    memory[HL_VALUE] = 0x0E;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0xF1);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    cpu->reg.a = 0x3E;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xD6;
    memory[1] = 0x0F;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x2F);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    cpu->reg.a = 0x3E;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x96;
    memory[HL_VALUE] = 0x40;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0xFE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_SBC)
{
    uint cycles = 0;
    cpu->reg.af = 0x12A1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    ResetState();
    cpu->reg.a = 0x3B;
    cpu->reg.flags.c = 1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = 0x2ADE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x9C;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x10);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, 0x2ADE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    ResetState();
    cpu->reg.a = 0x3B;
    cpu->reg.flags.c = 1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xDE;
    memory[1] = 0x3A;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x00);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    cpu->reg.a = 0x3B;
    cpu->reg.flags.c = 1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x9E;
    memory[HL_VALUE] = 0x4F;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0xEB);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_AND)
{
    uint cycles = 0;
    cpu->reg.af = 0x12A1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    ResetState();
    cpu->reg.a = 0x5A;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = 0xBC3F;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xA5;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x1A);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, 0xBC3F);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    ResetState();
    cpu->reg.a = 0x5A;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xE6;
    memory[1] = 0x38;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x18);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    cpu->reg.a = 0x5A;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xA6;
    memory[HL_VALUE] = 0x00;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x00);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_OR)
{
    uint cycles = 0;
    cpu->reg.af = 0x12A1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    cpu->reg.a = 0x5A;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xB7;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x5A);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    ResetState();
    cpu->reg.a = 0x5A;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xF6;
    memory[1] = 0x03;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x5B);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    cpu->reg.a = 0x5A;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xB6;
    memory[HL_VALUE] = 0x0F;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x5F);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_XOR)
{
    uint cycles = 0;
    cpu->reg.af = 0x12A1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    ResetState();
    cpu->reg.a = 0xFF;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xAF;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x00);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 4);

    ResetState();
    cpu->reg.a = 0xFF;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xEE;
    memory[1] = 0x0F;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0xF0);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    cpu->reg.a = 0xFF;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xAE;
    memory[HL_VALUE] = 0x8A;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x75);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_CP)
{
    uint cycles = 0;
    cpu->reg.af = 0x12A1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    ResetState();
    cpu->reg.a = 0x3C;
    cpu->reg.bc = 0x2F56;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xB8;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x3C);
    ASSERT_EQ(cpu->reg.bc, 0x2F56);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    ResetState();
    cpu->reg.a = 0x3C;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xFE;
    memory[1] = 0x3C;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x3C);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    cpu->reg.a = 0x3C;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0xBE;
    memory[HL_VALUE] = 0x40;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x3C);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_INC)
{
    uint cycles = 0;
    cpu->reg.af = 0x12A1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    ResetState();
    cpu->reg.a = 0xFF;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x3C;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x00);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 4);

    ResetState();
    cpu->reg.a = 0x3C;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x34;
    memory[HL_VALUE] = 0x50;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[cpu->reg.hl], 0x51);
    ASSERT_EQ(cpu->reg.a, 0x3C);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_DEC)
{
    uint cycles = 0;
    cpu->reg.af = 0x12A1;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    ResetState();
    cpu->reg.a = 0xFF;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = 0xBC01;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x2D;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0xFF);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, 0xBC00);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 4);

    ResetState();
    cpu->reg.a = 0x3C;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x35;
    memory[HL_VALUE] = 0x00;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[cpu->reg.hl], 0xFF);
    ASSERT_EQ(cpu->reg.a, 0x3C);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_ADD_HL_REG)
{
    uint cycles = 0;
    cpu->reg.af = 0x1200;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = HL_VALUE;
    cpu->reg.sp = SP_VALUE;

    ResetState();
    cpu->reg.a = 0xFF;
    cpu->reg.bc = 0x0605;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = 0x8A23;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x09;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0xFF);
    ASSERT_EQ(cpu->reg.bc, 0x0605);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, 0x9028);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    cpu->reg.flags.z = 1;
    cpu->reg.a = 0x3C;
    cpu->reg.bc = BC_VALUE;
    cpu->reg.de = DE_VALUE;
    cpu->reg.hl = 0x8A23;
    cpu->reg.sp = SP_VALUE;
    cpu->reg.pc = 0;
    memory[0] = 0x29;
    memory[HL_VALUE] = 0x00;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x3C);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, 0x1446);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_ADD_SP_N)
{
    uint cycles = 0;

    // ADD SP, N
    ResetState();
    memory[0] = 0xE8;
    memory[1] = 2;
    cpu->reg.sp = 0xFFF8;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, 0xFFFA);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    // ADD SP, N
    ResetState();
    memory[0] = 0xE8;
    memory[1] = 8;
    cpu->reg.sp = 0xFFF8;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.sp, 0);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0); // Zero isn't set.
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_INC_16)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x03;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE + 1);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0x13;
    cpu->reg.de = 0xFFFF;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, 0);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    // No flags should change.
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_DEC_16)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x2B;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE - 1);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0x3B;
    cpu->reg.sp = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, 0xFFFF);
    // No flags should change.
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RLCA)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x07;
    cpu->reg.a = 0x85;
    cpu->reg.f = 0xFF;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x0B);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RLA)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x17;
    cpu->reg.a = 0x95;
    cpu->reg.f = 0xFF;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x2B);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RRCA)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x0F;
    cpu->reg.a = 0x3B;
    cpu->reg.f = 0xFF;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x9D);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RRA)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x1F;
    cpu->reg.a = 0x81;
    cpu->reg.f = 0xFF;
    cpu->reg.flags.c = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x40);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RLC)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x00;
    cpu->reg.b = 0x85;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.b, 0x0B);
    ASSERT_EQ(cpu->reg.c, C_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x06;
    memory[HL_VALUE] = 0x00;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 0);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RRC)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x09;
    cpu->reg.c = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.b, B_VALUE);
    ASSERT_EQ(cpu->reg.c, 0x80);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x0E;
    memory[HL_VALUE] = 0x00;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 0);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RL)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x15;
    cpu->reg.l = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.h, H_VALUE);
    ASSERT_EQ(cpu->reg.l, 0x00);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x16;
    memory[HL_VALUE] = 0x11;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 0x22);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RR)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x1F;
    cpu->reg.a = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x00);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x1E;
    memory[HL_VALUE] = 0x8A;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 0x45);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_SLA)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x22;
    cpu->reg.d = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.d, 0x00);
    ASSERT_EQ(cpu->reg.e, E_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x26;
    memory[HL_VALUE] = 0xFF;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 0xFE);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_SRA)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x2A;
    cpu->reg.d = 0x8A;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.d, 0xC5);
    ASSERT_EQ(cpu->reg.e, E_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x2E;
    memory[HL_VALUE] = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 0x00);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_SWAP)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x37;
    cpu->reg.a = 0x00;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x36;
    memory[HL_VALUE] = 0xF0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 0x0F);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_SRL)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x3F;
    cpu->reg.a = 0x01;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x3E;
    memory[HL_VALUE] = 0xFF;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 0x7F);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_BIT)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x7F;
    cpu->reg.a = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x80);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x65;
    cpu->reg.l = 0xEF;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.h, H_VALUE);
    ASSERT_EQ(cpu->reg.l, 0xEF);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x46;
    memory[HL_VALUE] = 0xFE;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 0xFE);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 12);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x4E;
    memory[HL_VALUE] = 0xFE;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 0xFE);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RES)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0xBF;
    cpu->reg.a = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x00);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x8D;
    cpu->reg.l = 0x3B;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.h, H_VALUE);
    ASSERT_EQ(cpu->reg.l, 0x39);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0x9E;
    memory[HL_VALUE] = 0xFF;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 0xF7);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_SET)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0xDF;
    cpu->reg.a = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0x88);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0xFD;
    cpu->reg.l = 0x3B;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.h, H_VALUE);
    ASSERT_EQ(cpu->reg.l, 0xBB);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);

    ResetState();
    memory[0] = 0xCB;
    memory[1] = 0xDE;
    memory[HL_VALUE] = 0x00;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(memory[HL_VALUE], 0x08);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_JP)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xC3;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_JP_NZ)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xC2;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0] = 0xC2;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->reg.flags.z = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_JP_Z)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCA;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->reg.flags.z = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0] = 0xCA;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->reg.flags.z = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_JP_NC)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xD2;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0] = 0xD2;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->reg.flags.c = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_JP_C)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xDA;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->reg.flags.c = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0] = 0xDA;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->reg.flags.c = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_JR)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x18;
    memory[1] = 0x05;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0007);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    ResetState();
    memory[0xF0] = 0x18;
    memory[0xF1] = 0xFB;
    cpu->reg.pc = 0xF0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x00ED);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_JR_NZ)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x20;
    memory[1] = 0x0F;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0011);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    ResetState();
    memory[0] = 0x20;
    memory[1] = 0x0F;
    cpu->reg.flags.z = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_JR_Z)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x28;
    memory[1] = 0x0F;
    cpu->reg.flags.z = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0011);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 12);

    ResetState();
    memory[0] = 0x28;
    memory[1] = 0x0F;
    cpu->reg.flags.z = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_JR_NC)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x30;
    memory[1] = 0x0F;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0011);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    ResetState();
    memory[0] = 0x30;
    memory[1] = 0x0F;
    cpu->reg.flags.c = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_JR_C)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x38;
    memory[1] = 0x0F;
    cpu->reg.flags.c = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0011);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);

    ResetState();
    memory[0] = 0x38;
    memory[1] = 0x0F;
    cpu->reg.flags.c = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_JP_HL)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xE9;
    cpu->reg.hl = 0x8001;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, 0x8001);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_CALL)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCD;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x00);
    ASSERT_EQ(memory[0xFFFC], 0x03);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 24);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_CALL_NZ)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xC4;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x00);
    ASSERT_EQ(memory[0xFFFC], 0x03);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 24);

    ResetState();
    memory[0] = 0xC4;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->reg.flags.z = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_CALL_Z)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xCC;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->reg.flags.z = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x00);
    ASSERT_EQ(memory[0xFFFC], 0x03);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 24);

    ResetState();
    memory[0] = 0xCC;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->reg.flags.z = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_CALL_NC)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xD4;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x00);
    ASSERT_EQ(memory[0xFFFC], 0x03);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 24);

    ResetState();
    memory[0] = 0xD4;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->reg.flags.c = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_CALL_C)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xDC;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->reg.flags.c = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x00);
    ASSERT_EQ(memory[0xFFFC], 0x03);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 24);

    ResetState();
    memory[0] = 0xDC;
    memory[1] = 0x01;
    memory[2] = 0x80;
    cpu->reg.flags.c = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0003);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 12);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RET)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xC9;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
    cpu->reg.sp = 0xFFFC;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RETI)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xD9;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
    cpu->reg.sp = 0xFFFC;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(interrupts->Enabled(), true);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RET_NZ)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xC0;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
    cpu->reg.sp = 0xFFFC;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 20);

    ResetState();
    memory[0] = 0xC0;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
    cpu->reg.sp = 0xFFFC;
    cpu->reg.flags.z = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RET_Z)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xC8;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
    cpu->reg.sp = 0xFFFC;
    cpu->reg.flags.z = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 1);
    ASSERT_EQ(cycles, 20);

    ResetState();
    memory[0] = 0xC8;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
    cpu->reg.sp = 0xFFFC;
    cpu->reg.flags.z = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RET_NC)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xD0;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
    cpu->reg.sp = 0xFFFC;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 20);

    ResetState();
    memory[0] = 0xD0;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
    cpu->reg.sp = 0xFFFC;
    cpu->reg.flags.c = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RET_C)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0xD8;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
    cpu->reg.sp = 0xFFFC;
    cpu->reg.flags.c = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x8001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 20);

    ResetState();
    memory[0] = 0xD8;
    memory[0xFFFD] = 0x80;
    memory[0xFFFC] = 0x01;
    cpu->reg.sp = 0xFFFC;
    cpu->reg.flags.c = 0;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 8);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_RST)
{
    uint cycles = 0;

    ResetState();
    memory[0x1234] = 0xC7;
    cpu->reg.pc = 0x1234;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0000);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xCF;
    cpu->reg.pc = 0x1234;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0008);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xD7;
    cpu->reg.pc = 0x1234;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0010);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xDF;
    cpu->reg.pc = 0x1234;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0018);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xE7;
    cpu->reg.pc = 0x1234;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0020);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xEF;
    cpu->reg.pc = 0x1234;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0028);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xF7;
    cpu->reg.pc = 0x1234;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0030);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);

    ResetState();
    memory[0x1234] = 0xFF;
    cpu->reg.pc = 0x1234;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0038);
    ASSERT_EQ(cpu->reg.sp, 0xFFFC);
    ASSERT_EQ(memory[0xFFFD], 0x12);
    ASSERT_EQ(memory[0xFFFC], 0x35);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 16);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_CPL)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x2F;
    cpu->reg.a = 0x35;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, 0xCA);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 1);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_NOP)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x00;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_SCF)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x37;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    ResetState();
    memory[0] = 0x37;
    cpu->reg.flags.c = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_CCF)
{
    uint cycles = 0;

    ResetState();
    memory[0] = 0x3F;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 1);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    ResetState();
    memory[0] = 0x3F;
    cpu->reg.flags.c = 1;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_DAA)
{
    uint cycles = 0;

    ResetState();
    cpu->reg.a = 0x45;
    cpu->reg.b = 0x38;
    memory[0] = 0x80; // ADD A, B
    memory[1] = 0x27; // DAA
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cycles, 4);
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cycles, 8);
    ASSERT_EQ(cpu->reg.a, 0x83);
    ASSERT_EQ(cpu->reg.b, 0x38);
    ASSERT_EQ(cpu->reg.c, C_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);

    ResetState();
    cpu->reg.a = 0x83;
    cpu->reg.b = 0x38;
    memory[0] = 0x90; // SUB A, B
    memory[1] = 0x27; // DAA
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cycles, 4);
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cycles, 8);
    ASSERT_EQ(cpu->reg.a, 0x45);
    ASSERT_EQ(cpu->reg.b, 0x38);
    ASSERT_EQ(cpu->reg.c, C_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 1);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_DI)
{
    uint cycles = 0;

    ResetState();
    interrupts->SetEnabled(true);
    memory[0] = 0xF3;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(interrupts->Enabled(), false);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);

    ResetState();
    interrupts->SetEnabled(false);
    memory[0] = 0xF3;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(interrupts->Enabled(), false);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
    ASSERT_EQ(cycles, 4);
}


///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_EI)
{
    uint cycles = 0;

    ResetState();
    interrupts->SetEnabled(false);
    memory[0] = 0xFB;
    memory[1] = 0x00;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cycles, 4);
    ASSERT_EQ(interrupts->Enabled(), false); // EI has a delayed effect.
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cycles, 8);
    ASSERT_EQ(interrupts->Enabled(), true);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);

    ResetState();
    interrupts->SetEnabled(true);
    memory[0] = 0xFB;
    memory[1] = 0x00;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cycles, 4);
    ASSERT_EQ(interrupts->Enabled(), true); // EI has a delayed effect, but it was already enabled.
    cpu->ProcessOpCode();
    cycles = timer->GetCounter();
    ASSERT_EQ(cycles, 8);
    ASSERT_EQ(interrupts->Enabled(), true);
    ASSERT_EQ(cpu->reg.a, A_VALUE);
    ASSERT_EQ(cpu->reg.bc, BC_VALUE);
    ASSERT_EQ(cpu->reg.de, DE_VALUE);
    ASSERT_EQ(cpu->reg.hl, HL_VALUE);
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cpu->reg.flags.c, 0);
    ASSERT_EQ(cpu->reg.flags.h, 0);
    ASSERT_EQ(cpu->reg.flags.n, 0);
    ASSERT_EQ(cpu->reg.flags.z, 0);
}

///////////////////////////////////////////////////////////////////////////////

TEST_F(CpuTest, Test_Interrupts)
{
    uint cycles = 0;

    ResetState();
    interrupts->SetEnabled(true);
    memory[eRegIE] = 0x01;
    memory[eRegIF] = 0x01;
    memory[0] = 0x00;
    memory[1] = 0x00;
    memory[0x40] = 0x3C; // INC A
    memory[0x41] = 0xC9; // RET
    cpu->ProcessOpCode();
    cycles = timer->GetCounter(); // Process interrupt
    ASSERT_EQ(cpu->reg.pc, 0x0040);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE - 2);
    ASSERT_EQ(cycles, 20);
    
    cpu->ProcessOpCode();
    cycles = timer->GetCounter(); // Process INC A
    ASSERT_EQ(cpu->reg.pc, 0x0041);
    ASSERT_EQ(cpu->reg.a, A_VALUE + 1);
    ASSERT_EQ(cycles, 24);

    cpu->ProcessOpCode();
    cycles = timer->GetCounter(); // Process RET
    ASSERT_EQ(cpu->reg.pc, 0x0000);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cycles, 40);


    ResetState();
    interrupts->SetEnabled(true);
    memory[eRegIE] = 0x00;
    memory[eRegIF] = 0x01;
    memory[0] = 0x00;
    memory[1] = 0x00;
    memory[0x40] = 0x3C; // INC A
    memory[0x41] = 0xC9; // RET
    cpu->ProcessOpCode();
    cycles = timer->GetCounter(); // IE disabled
    ASSERT_EQ(cpu->reg.pc, 0x0001);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cycles, 4);
    
    cpu->ProcessOpCode();
    cycles = timer->GetCounter(); // IE disabled
    ASSERT_EQ(cpu->reg.pc, 0x0002);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE);
    ASSERT_EQ(cycles, 8);


    ResetState();
    interrupts->SetEnabled(true);
    memory[eRegIE] = 0x02;
    memory[eRegIF] = 0x02;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter(); // Process interrupt
    ASSERT_EQ(cpu->reg.pc, 0x0048);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE - 2);
    ASSERT_EQ(cycles, 20);

    ResetState();
    interrupts->SetEnabled(true);
    memory[eRegIE] = 0x04;
    memory[eRegIF] = 0x04;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter(); // Process interrupt
    ASSERT_EQ(cpu->reg.pc, 0x0050);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE - 2);
    ASSERT_EQ(cycles, 20);

    ResetState();
    interrupts->SetEnabled(true);
    memory[eRegIE] = 0x08;
    memory[eRegIF] = 0x08;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter(); // Process interrupt
    ASSERT_EQ(cpu->reg.pc, 0x0058);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE - 2);
    ASSERT_EQ(cycles, 20);

    ResetState();
    interrupts->SetEnabled(true);
    memory[eRegIE] = 0x10;
    memory[eRegIF] = 0x10;
    cpu->ProcessOpCode();
    cycles = timer->GetCounter(); // Process interrupt
    ASSERT_EQ(cpu->reg.pc, 0x0060);
    ASSERT_EQ(cpu->reg.sp, SP_VALUE - 2);
    ASSERT_EQ(cycles, 20);
}
