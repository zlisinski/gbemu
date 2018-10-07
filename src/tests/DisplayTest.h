#pragma once

#include <gtest/gtest.h>
#include "../Display.h"
#include "../State.h"

class DisplayTest : public ::testing::Test
{
protected:
    DisplayTest();
    ~DisplayTest() override;

    void SetUp() override;
    void TearDown() override;

    State *state;
};