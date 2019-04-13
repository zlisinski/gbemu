#pragma once

#include <gtest/gtest.h>
#include "../MemoryBankController.h"

class MbcTest : public ::testing::Test
{
protected:
    MbcTest();
    ~MbcTest() override;

    void SetUp() override;
    void TearDown() override;
};