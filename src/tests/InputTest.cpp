#include "InputTest.h"


InputTest::InputTest()
{

}


InputTest::~InputTest()
{

}


void InputTest::SetUp()
{
    state.memory->ClearMemory();
}


void InputTest::TearDown()
{

}


TEST_F(InputTest, TEST_PressUp)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);

    regP1 = 0xEF;
    input.SetButtonUp(true);
    ASSERT_EQ(0xEB, regP1);

    regP1 = 0xDF;
    input.SetButtonUp(true);
    ASSERT_EQ(0xDF, regP1);

    regP1 = 0xFF;
    input.SetButtonUp(true);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressDown)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);

    regP1 = 0xEF;
    input.SetButtonDown(true);
    ASSERT_EQ(0xE7, regP1);

    regP1 = 0xDF;
    input.SetButtonDown(true);
    ASSERT_EQ(0xDF, regP1);

    regP1 = 0xFF;
    input.SetButtonDown(true);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressLeft)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);

    regP1 = 0xEF;
    input.SetButtonLeft(true);
    ASSERT_EQ(0xED, regP1);

    regP1 = 0xDF;
    input.SetButtonLeft(true);
    ASSERT_EQ(0xDF, regP1);

    regP1 = 0xFF;
    input.SetButtonLeft(true);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressRight)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);

    regP1 = 0xEF;
    input.SetButtonRight(true);
    ASSERT_EQ(0xEE, regP1);

    regP1 = 0xDF;
    input.SetButtonRight(true);
    ASSERT_EQ(0xDF, regP1);

    regP1 = 0xFF;
    input.SetButtonRight(true);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressStart)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);

    regP1 = 0xDF;
    input.SetButtonStart(true);
    ASSERT_EQ(0xD7, regP1);

    regP1 = 0xEF;
    input.SetButtonStart(true);
    ASSERT_EQ(0xEF, regP1);

    regP1 = 0xFF;
    input.SetButtonStart(true);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressSelect)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);

    regP1 = 0xDF;
    input.SetButtonSelect(true);
    ASSERT_EQ(0xDB, regP1);

    regP1 = 0xEF;
    input.SetButtonSelect(true);
    ASSERT_EQ(0xEF, regP1);

    regP1 = 0xFF;
    input.SetButtonSelect(true);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressB)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);

    regP1 = 0xDF;
    input.SetButtonB(true);
    ASSERT_EQ(0xDD, regP1);

    regP1 = 0xEF;
    input.SetButtonB(true);
    ASSERT_EQ(0xEF, regP1);

    regP1 = 0xFF;
    input.SetButtonB(true);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressA)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);

    regP1 = 0xDF;
    input.SetButtonA(true);
    ASSERT_EQ(0xDE, regP1);

    regP1 = 0xEF;
    input.SetButtonA(true);
    ASSERT_EQ(0xEF, regP1);

    regP1 = 0xFF;
    input.SetButtonA(true);
    ASSERT_EQ(0xFF, regP1);
}