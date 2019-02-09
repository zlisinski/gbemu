#pragma once

#include <gtest/gtest.h>
#include "../Input.h"

class Memory;

class InputTest : public ::testing::Test
{
protected:
    InputTest();
    ~InputTest() override;

    void SetUp() override;
    void TearDown() override;

    Memory *memory;
};