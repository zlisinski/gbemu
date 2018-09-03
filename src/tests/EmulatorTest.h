#pragma once

#include <gtest/gtest.h>
#include "../State.h"

class EmulatorTest : public ::testing::Test
{
protected:
    EmulatorTest();
    ~EmulatorTest() override;

    void SetUp() override;
    void TearDown() override;

    void ResetState();

    State state;
    uint8_t *memory;
};