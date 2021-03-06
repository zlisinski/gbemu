#pragma once

#include <gtest/gtest.h>

class Cpu;
class Interrupt;
class Memory;
class Timer;

class CpuTest : public ::testing::Test
{
protected:
    CpuTest();
    ~CpuTest() override;

    void SetUp() override;
    void TearDown() override;

    void ResetState();

    Cpu *cpu;
    Memory *memory_;
    Timer *timer;
    Interrupt *interrupts;
    
    uint8_t *memory;
};