#include "main.h"
#include "InputTest.h"
#include "../Memory.h"


InputTest::InputTest()
{
    memory = new Memory(NULL);
}


InputTest::~InputTest()
{
    delete memory;
}


void InputTest::SetUp()
{
    memory->ClearMemory();
}


void InputTest::TearDown()
{

}


TEST_F(InputTest, TEST_PressUp)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);
    Buttons buttons;
    buttons.data = Buttons::eButtonUp;

    regP1 = 0xEF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xEB, regP1);

    regP1 = 0xDF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xDF, regP1);

    regP1 = 0xFF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressDown)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);
    Buttons buttons;
    buttons.data = Buttons::eButtonDown;

    regP1 = 0xEF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xE7, regP1);

    regP1 = 0xDF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xDF, regP1);

    regP1 = 0xFF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressLeft)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);
    Buttons buttons;
    buttons.data = Buttons::eButtonLeft;

    regP1 = 0xEF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xED, regP1);

    regP1 = 0xDF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xDF, regP1);

    regP1 = 0xFF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressRight)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);
    Buttons buttons;
    buttons.data = Buttons::eButtonRight;

    regP1 = 0xEF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xEE, regP1);

    regP1 = 0xDF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xDF, regP1);

    regP1 = 0xFF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressStart)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);
    Buttons buttons;
    buttons.data = Buttons::eButtonStart;

    regP1 = 0xDF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xD7, regP1);

    regP1 = 0xEF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xEF, regP1);

    regP1 = 0xFF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressSelect)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);
    Buttons buttons;
    buttons.data = Buttons::eButtonSelect;

    regP1 = 0xDF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xDB, regP1);

    regP1 = 0xEF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xEF, regP1);

    regP1 = 0xFF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressB)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);
    Buttons buttons;
    buttons.data = Buttons::eButtonB;

    regP1 = 0xDF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xDD, regP1);

    regP1 = 0xEF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xEF, regP1);

    regP1 = 0xFF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xFF, regP1);
}


TEST_F(InputTest, TEST_PressA)
{
    uint8_t regP1 = 0;
    Input input(&regP1, NULL);
    Buttons buttons;
    buttons.data = Buttons::eButtonA;

    regP1 = 0xDF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xDE, regP1);

    regP1 = 0xEF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xEF, regP1);

    regP1 = 0xFF;
    input.SetButtons(buttons);
    ASSERT_EQ(0xFF, regP1);
}