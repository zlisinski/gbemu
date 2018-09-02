#include <string>

#include "MemoryTest.h"

MemoryTest::MemoryTest()
{

}

MemoryTest::~MemoryTest()
{

}

void MemoryTest::SetUp()
{
    memory.ClearMemory();
}

void MemoryTest::TearDown()
{

}

TEST_F(MemoryTest, TEST_Serial)
{
    testing::internal::CaptureStdout();

    memory.WriteByte(MEM_SB, 0x30);
    memory.WriteByte(MEM_SC, 0x81);
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "Serial: 30, '0'\n");
}