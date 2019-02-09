#pragma once

#include <gtest/gtest.h>
#include "../State.h"
#include "../Cpu.h"

class CpuTest : public ::testing::Test
{
protected:
    CpuTest();
    ~CpuTest() override;

    void SetUp() override;
    void TearDown() override;

    void ResetState();

    State state;
    Cpu cpu;
    uint8_t *memory;
};