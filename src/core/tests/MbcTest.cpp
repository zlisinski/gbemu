#include <iomanip>
#include <sstream>
#include <string>

#include "MbcTest.h"

class MemoryInterfaceMock : public MemoryBankInterface
{
public:
    MemoryInterfaceMock()
    {
        Reset();
    }

    virtual void MapRomBank(uint8_t bank)
    {
        romBank = bank;
    }

    virtual void MapRamBank(uint8_t bank)
    {
        ramBank = bank;
    }

    virtual void EnableRam(bool enable)
    {
        ramEnabled = enable;
    }

    void Reset()
    {
        ramEnabled = false;
        ramBank = 0;
        romBank = 0;
    }

    bool ramEnabled;
    uint8_t ramBank;
    uint8_t romBank;
};


MbcTest::MbcTest()
{

}

MbcTest::~MbcTest()
{

}

void MbcTest::SetUp()
{

}

void MbcTest::TearDown()
{

}


// Mbc1 ///////////////////////////////////////////////////////////////////////////////////////////


TEST_F(MbcTest, TEST_Mbc1_Ram_Enable)
{
    MemoryInterfaceMock memory;
    Mbc1 mbc(&memory);

    // Test that 0xnA is the only value that enables RAM.
    for (uint i = 0; i < 0x100; i++)
    {
        memory.Reset();
        ASSERT_FALSE(memory.ramEnabled);
        mbc.WriteByte(0, i);

        std::stringstream ss;
        ss << "i == " << std::hex << std::setw(2) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        if ((i & 0x0F) == 0x0A)
            ASSERT_TRUE(memory.ramEnabled);
        else
            ASSERT_FALSE(memory.ramEnabled);
    }

    // Test range for RAM enable.
    for (uint i = 0; i < 0x10000; i++)
    {
        memory.Reset();
        ASSERT_FALSE(memory.ramEnabled);
        mbc.WriteByte(i, 0x0A);

        std::stringstream ss;
        ss << "i == 0x" << std::hex << std::setw(4) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        if (i < 0x2000)
            ASSERT_TRUE(memory.ramEnabled);
        else
            ASSERT_FALSE(memory.ramEnabled);
    }
}


TEST_F(MbcTest, TEST_Mbc1_Rom_Bank_Low)
{
    MemoryInterfaceMock memory;
    Mbc1 mbc(&memory);

    // Test mapping banks.
    for (uint i = 0; i < 0x100; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.romBank, 0);
        mbc.WriteByte(0x2000, i);

        std::stringstream ss;
        ss << "i == " << std::hex << std::setw(2) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());

        // Mbc1 in mode 0 supports 0x20 banks.
        // If bank % 0x20 is 0, mapped bank will be 1.
        if (i & 0x1F)
            ASSERT_EQ(memory.romBank, i & 0x1F);
        else
            ASSERT_EQ(memory.romBank, 1);
    }

    // Test range for ROM bank.
    for (uint i = 0; i < 0x4000; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.romBank, 0);
        mbc.WriteByte(i, 2);

        std::stringstream ss;
        ss << "i == 0x" << std::hex << std::setw(4) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        if (i >= 0x2000 && i < 0x4000)
            ASSERT_EQ(memory.romBank, 2);
        else
            ASSERT_EQ(memory.romBank, 0);
    }
}


TEST_F(MbcTest, TEST_Mbc1_Rom_Bank_High)
{
    MemoryInterfaceMock memory;
    Mbc1 mbc(&memory);

    // Test mapping banks.
    for (uint i = 0; i < 0x100; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.romBank, 0);
        mbc.WriteByte(0x4000, i);

        std::stringstream ss;
        ss << "i == " << std::hex << std::setw(2) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        if (i)
            ASSERT_EQ(memory.romBank, ((i & 0x03) << 5) | 0x01);
        else
            ASSERT_EQ(memory.romBank, 1);
    }

    // Test range for ROM bank.
    for (uint i = 0x4000; i < 0x6000; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.romBank, 0);
        mbc.WriteByte(i, 1);

        std::stringstream ss;
        ss << "i == 0x" << std::hex << std::setw(4) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        if (i >= 0x4000 && i < 0x6000)
            ASSERT_EQ(memory.romBank, (1 << 5) | 1);
        else
            ASSERT_EQ(memory.romBank, 0);
    }
}


TEST_F(MbcTest, TEST_Mbc1_Ram_Mode)
{
    MemoryInterfaceMock memory;
    Mbc1 mbc(&memory);

    // Test mapping RAM banks.
    for (uint i = 0; i < 0x100; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.ramBank, 0);
        mbc.WriteByte(0x6000, 1);
        mbc.WriteByte(0x4000, i);

        std::stringstream ss;
        ss << "i == " << std::hex << std::setw(2) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());

        ASSERT_EQ(memory.ramBank, (i & 0x03));
    }

    // Test range for RAM bank.
    for (uint i = 0x6000; i < 0x8000; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.ramBank, 0);
        mbc.WriteByte(i, 1);
        mbc.WriteByte(0x4000, 1);

        std::stringstream ss;
        ss << "i == 0x" << std::hex << std::setw(4) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        ASSERT_EQ(memory.ramBank, 1);
    }
}


TEST_F(MbcTest, TEST_Mbc1_Unmapped_memory)
{
    MemoryInterfaceMock memory;
    Mbc1 mbc(&memory);

    for (uint i = 0x8000; i < 0x10000; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.ramEnabled, 0);
        ASSERT_EQ(memory.ramBank, 0);
        ASSERT_EQ(memory.romBank, 0);
        
        mbc.WriteByte(i, 1);
        ASSERT_EQ(memory.ramEnabled, 0);
        ASSERT_EQ(memory.ramBank, 0);
        ASSERT_EQ(memory.romBank, 0);
    }
}


// Mbc2 ///////////////////////////////////////////////////////////////////////////////////////////



TEST_F(MbcTest, TEST_Mbc2_Ram_Enable)
{
    MemoryInterfaceMock memory;
    Mbc2 mbc(&memory);

    // Test that 0xnA is the only value that enables RAM.
    for (uint i = 0; i < 0x100; i++)
    {
        memory.Reset();
        ASSERT_FALSE(memory.ramEnabled);
        mbc.WriteByte(0, i);

        std::stringstream ss;
        ss << "i == " << std::hex << std::setw(2) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        if ((i & 0x0F) == 0x0A)
            ASSERT_TRUE(memory.ramEnabled);
        else
            ASSERT_FALSE(memory.ramEnabled);
    }

    // Test range for RAM enable.
    for (uint i = 0; i < 0x10000; i++)
    {
        memory.Reset();
        ASSERT_FALSE(memory.ramEnabled);
        mbc.WriteByte(i, 0x0A);

        std::stringstream ss;
        ss << "i == 0x" << std::hex << std::setw(4) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        if (i < 0x2000 && (i & 0x0100) == 0)
            ASSERT_TRUE(memory.ramEnabled);
        else
            ASSERT_FALSE(memory.ramEnabled);
    }
}


TEST_F(MbcTest, TEST_Mbc2_Rom_Bank)
{
    MemoryInterfaceMock memory;
    Mbc2 mbc(&memory);

    // Test mapping banks.
    for (uint i = 0; i < 0x100; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.romBank, 0);
        mbc.WriteByte(0x2100, i);

        std::stringstream ss;
        ss << "i == " << std::hex << std::setw(2) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());

        // Mbc2 supports 0x10 banks.
        // If bank % 16 is 0, mapped bank will be 1.
        if (i & 0x0F)
            ASSERT_EQ(memory.romBank, i & 0x0F);
        else
            ASSERT_EQ(memory.romBank, 1);
    }

    // Test range for ROM bank.
    for (uint i = 0; i < 0x10000; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.romBank, 0);
        mbc.WriteByte(i, 2);

        std::stringstream ss;
        ss << "i == 0x" << std::hex << std::setw(4) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        if (i >= 0x2000 && i < 0x4000 && (i & 0x0100) == 0x0100)
            ASSERT_EQ(memory.romBank, 2);
        else
            ASSERT_EQ(memory.romBank, 0);
    }
}


// Mbc3 ///////////////////////////////////////////////////////////////////////////////////////////


TEST_F(MbcTest, TEST_Mbc3_Ram_Enable)
{
    MemoryInterfaceMock memory;
    Mbc3 mbc(&memory);

    // Test that 0xnA is the only value that enables RAM.
    for (uint i = 0; i < 0x100; i++)
    {
        memory.Reset();
        ASSERT_FALSE(memory.ramEnabled);
        mbc.WriteByte(0, i);

        std::stringstream ss;
        ss << "i == " << std::hex << std::setw(2) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        if ((i & 0x0F) == 0x0A)
            ASSERT_TRUE(memory.ramEnabled);
        else
            ASSERT_FALSE(memory.ramEnabled);
    }

    // Test range for RAM enable.
    for (uint i = 0; i < 0x10000; i++)
    {
        memory.Reset();
        ASSERT_FALSE(memory.ramEnabled);
        mbc.WriteByte(i, 0x0A);

        std::stringstream ss;
        ss << "i == 0x" << std::hex << std::setw(4) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        if (i < 0x2000)
            ASSERT_TRUE(memory.ramEnabled);
        else
            ASSERT_FALSE(memory.ramEnabled);
    }
}


TEST_F(MbcTest, TEST_Mbc3_Rom_Bank)
{
    MemoryInterfaceMock memory;
    Mbc3 mbc(&memory);

    // Test mapping banks.
    for (uint i = 0; i < 0x100; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.romBank, 0);
        mbc.WriteByte(0x2000, i);

        std::stringstream ss;
        ss << "i == " << std::hex << std::setw(2) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());

        // Mbc3 supports 0x7F banks.
        // If bank % 128 is 0, mapped bank will be 1.
        if (i & 0x7F)
            ASSERT_EQ(memory.romBank, i & 0x7F);
        else
            ASSERT_EQ(memory.romBank, 1);
    }

    // Test range for ROM bank.
    for (uint i = 0; i < 0x10000; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.romBank, 0);
        mbc.WriteByte(i, 2);

        std::stringstream ss;
        ss << "i == 0x" << std::hex << std::setw(4) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        if (i >= 0x2000 && i < 0x4000)
            ASSERT_EQ(memory.romBank, 2);
        else
            ASSERT_EQ(memory.romBank, 0);
    }
}


TEST_F(MbcTest, TEST_Mbc3_Ram_Bank)
{
    MemoryInterfaceMock memory;
    Mbc3 mbc(&memory);

    // Test mapping banks.
    for (uint i = 0; i < 0x100; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.ramBank, 0);
        mbc.WriteByte(0x4000, i);

        std::stringstream ss;
        ss << "i == " << std::hex << std::setw(2) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());

        // Mbc3 supports 4 banks.
        ASSERT_EQ(memory.ramBank, i & 0x03);
    }

    // Test range for RAM bank.
    for (uint i = 0; i < 0x10000; i++)
    {
        memory.Reset();
        ASSERT_EQ(memory.ramBank, 0);
        mbc.WriteByte(i, 2);

        std::stringstream ss;
        ss << "i == 0x" << std::hex << std::setw(4) << std::setfill('0') << i;
        SCOPED_TRACE(ss.str());
        if (i >= 0x4000 && i < 0x6000)
            ASSERT_EQ(memory.ramBank, 2);
        else
            ASSERT_EQ(memory.ramBank, 0);
    }
}