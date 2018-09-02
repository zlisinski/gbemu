#pragma once

#include <gtest/gtest.h>
#include "../Memory.h"

class MemoryTest : public ::testing::Test
{
protected:
    MemoryTest();
    virtual ~MemoryTest();

    virtual void SetUp();
    virtual void TearDown();

    Memory memory;
};