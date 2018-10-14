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
    //memory.ClearMemory();
}

void MemoryTest::TearDown()
{

}

/*TEST_F(MemoryTest, TEST_Serial)
{
    testing::internal::CaptureStdout();

    memory.WriteByte(eRegSB, 0x30);
    memory.WriteByte(eRegSC, 0x81);
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, "Serial: 30, '0'\n");
}*/


TEST_F(MemoryTest, TEST_SetRomMemory_memcpy)
{
    std::array<uint8_t, BOOT_ROM_SIZE> bootRomMemory;
    std::vector<uint8_t> gameRomMemory(ROM_BANK_SIZE * 2);
    memset(bootRomMemory.data(), 0x11, BOOT_ROM_SIZE);
    memset(gameRomMemory.data(), 0x22, gameRomMemory.size());

    Memory memory;
    memory.SetRomMemory(bootRomMemory, gameRomMemory);

    ASSERT_EQ(memory[0], 0x11);
    ASSERT_EQ(memory[0xFF], 0x11);
    ASSERT_EQ(memory[0x100], 0x22);
    ASSERT_EQ(memory[ROM_BANK_SIZE * 2], 0);
}


TEST_F(MemoryTest, TEST_SetRomMemory_memcpy2)
{
    std::vector<uint8_t> gameRomMemory(ROM_BANK_SIZE * 2);
    memset(gameRomMemory.data(), 0x22, gameRomMemory.size());

    Memory memory;
    memory.SetRomMemory(gameRomMemory);

    ASSERT_EQ(memory[0], 0x22);
    ASSERT_EQ(memory[0xFF], 0x22);
    ASSERT_EQ(memory[0x100], 0x22);
    ASSERT_EQ(memory[ROM_BANK_SIZE * 2], 0);
}


TEST_F(MemoryTest, TEST_DisableBootRom)
{
    std::array<uint8_t, BOOT_ROM_SIZE> bootRomMemory;
    std::vector<uint8_t> gameRomMemory(ROM_BANK_SIZE * 2);
    memset(bootRomMemory.data(), 0x11, BOOT_ROM_SIZE);
    memset(gameRomMemory.data(), 0x22, gameRomMemory.size());

    Memory memory;
    memory.SetRomMemory(bootRomMemory, gameRomMemory);

    ASSERT_EQ(memory[0xFF], 0x11);
    ASSERT_EQ(memory[0x100], 0x22);

    memory.WriteByte(eRegBootDisable, 1);

    ASSERT_EQ(memory[0x00], 0x22);
    ASSERT_EQ(memory[0xFF], 0x22);
    ASSERT_EQ(memory[0x100], 0x22);
}