#pragma once

#include <gtest/gtest.h>
#include "../Input.h"
#include "../State.h"

class InputTest : public ::testing::Test
{
protected:
    InputTest();
    ~InputTest() override;

    void SetUp() override;
    void TearDown() override;

    State state;
};