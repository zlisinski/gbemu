#include <gtest/gtest.h>
#include "EmulatorTest.h"

bool debugOutput = false;

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}