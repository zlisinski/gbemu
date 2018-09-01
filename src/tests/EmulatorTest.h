#pragma once

#include <gtest/gtest.h>
#include "../State.h"

class EmulatorTest : public ::testing::Test
{
protected:
    EmulatorTest();
    virtual ~EmulatorTest();

    virtual void SetUp();
    virtual void TearDown();

    void ResetState();

    State state;
    uint8_t *memory;
};