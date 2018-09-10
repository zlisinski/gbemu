#pragma once

#include <gtest/gtest.h>
#include "../State.h"
#include "../Emulator.h"

class EmulatorTest : public ::testing::Test
{
protected:
    EmulatorTest();
    ~EmulatorTest() override;

    void SetUp() override;
    void TearDown() override;

    void ResetState();

    State state;
    Emulator emulator;
    uint8_t *memory;
};