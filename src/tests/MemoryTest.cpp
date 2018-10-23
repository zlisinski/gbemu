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

TEST_F(MemoryTest, TEST_DMA)
{
    Memory memory;
    const uint8_t dmaStart = 0x02;
    uint8_t *src = memory.GetBytePtr(dmaStart << 8);
    uint8_t *dest = memory.GetBytePtr(OAM_RAM_START);
    const uint8_t byte = 0x11;

    memset(src, byte, OAM_RAM_LEN);

    ASSERT_EQ(dest[0], 0);

    memory.WriteByte(eRegDMA, dmaStart);

    ASSERT_EQ(dest[0], 0);

    memory.UpdateTimer(4);

    ASSERT_EQ(dest[0], byte);
    ASSERT_EQ(dest[1], 0);

    memory.UpdateTimer(4);

    ASSERT_EQ(dest[0], byte);
    ASSERT_EQ(dest[1], byte);
    ASSERT_EQ(dest[2], 0);

    for (uint8_t i = 2; i < OAM_RAM_LEN; i++)
    {
        memory.UpdateTimer(4);
    }

    ASSERT_EQ(dest[OAM_RAM_LEN - 1], byte);
    ASSERT_EQ(dest[OAM_RAM_LEN], 0);
}

TEST_F(MemoryTest, TEST_Start_new_DMA_in_middle_of_DMA)
{
    Memory memory;
    uint8_t dmaStart = 0x02;
    uint8_t *src = memory.GetBytePtr(dmaStart << 8);
    uint8_t *dest = memory.GetBytePtr(OAM_RAM_START);
    uint8_t byte = 0x11;

    memset(src, byte, OAM_RAM_LEN);

    // Start first DMA and finish half of it.
    memory.WriteByte(eRegDMA, dmaStart);
    for (uint8_t i = 0; i < OAM_RAM_LEN / 2; i++)
        memory.UpdateTimer(4);

    ASSERT_EQ(dest[0], byte);
    ASSERT_EQ(dest[(OAM_RAM_LEN / 2) - 1], byte);
    ASSERT_EQ(dest[(OAM_RAM_LEN / 2)], 0);

    // Start second DMA.
    dmaStart = 0x03;
    src = memory.GetBytePtr(dmaStart << 8);
    uint8_t byte2 = 0x22;
    memset(src, byte2, OAM_RAM_LEN);
    memory.WriteByte(eRegDMA, dmaStart);
    memory.UpdateTimer(4);

    ASSERT_EQ(dest[0], byte2);
    ASSERT_EQ(dest[1], byte);

    for (uint8_t i = 1; i < OAM_RAM_LEN; i++)
        memory.UpdateTimer(4);

    ASSERT_EQ(dest[OAM_RAM_LEN - 1], byte2);
}