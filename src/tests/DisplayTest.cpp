#include <SDL2/SDL.h>
#include <string.h>

#include "main.h"
#include "DisplayTest.h"

DisplayTest::DisplayTest()
{

}

DisplayTest::~DisplayTest()
{

}

void DisplayTest::SetUp()
{
    /*if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    state = new State(DrawFrame);

    state->memory->ClearMemory();*/
}

void DisplayTest::TearDown()
{
    /*delete state;
    SDL_Quit();*/
}

/*TEST_F(DisplayTest, TEST_Display_logo)
{
    uint8_t *memory = state->memory->GetBytePtr(0);

    uint8_t bg_data[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\xF0\x00\xF0\x00\xFC\x00\xFC\x00\xFC\x00\xFC\x00\xF3\x00\xF3\x00"
        "\x3C\x00\x3C\x00\x3C\x00\x3C\x00\x3C\x00\x3C\x00\x3C\x00\x3C\x00"
        "\xF0\x00\xF0\x00\xF0\x00\xF0\x00\x00\x00\x00\x00\xF3\x00\xF3\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xCF\x00\xCF\x00"
        "\x00\x00\x00\x00\x0F\x00\x0F\x00\x3F\x00\x3F\x00\x0F\x00\x0F\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\xC0\x00\xC0\x00\x0F\x00\x0F\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF0\x00\xF0\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF3\x00\xF3\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC0\x00\xC0\x00"
        "\x03\x00\x03\x00\x03\x00\x03\x00\x03\x00\x03\x00\xFF\x00\xFF\x00"
        "\xC0\x00\xC0\x00\xC0\x00\xC0\x00\xC0\x00\xC0\x00\xC3\x00\xC3\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFC\x00\xFC\x00"
        "\xF3\x00\xF3\x00\xF0\x00\xF0\x00\xF0\x00\xF0\x00\xF0\x00\xF0\x00"
        "\x3C\x00\x3C\x00\xFC\x00\xFC\x00\xFC\x00\xFC\x00\x3C\x00\x3C\x00"
        "\xF3\x00\xF3\x00\xF3\x00\xF3\x00\xF3\x00\xF3\x00\xF3\x00\xF3\x00"
        "\xF3\x00\xF3\x00\xC3\x00\xC3\x00\xC3\x00\xC3\x00\xC3\x00\xC3\x00"
        "\xCF\x00\xCF\x00\xCF\x00\xCF\x00\xCF\x00\xCF\x00\xCF\x00\xCF\x00"
        "\x3C\x00\x3C\x00\x3F\x00\x3F\x00\x3C\x00\x3C\x00\x0F\x00\x0F\x00"
        "\x3C\x00\x3C\x00\xFC\x00\xFC\x00\x00\x00\x00\x00\xFC\x00\xFC\x00"
        "\xFC\x00\xFC\x00\xF0\x00\xF0\x00\xF0\x00\xF0\x00\xF0\x00\xF0\x00"
        "\xF3\x00\xF3\x00\xF3\x00\xF3\x00\xF3\x00\xF3\x00\xF0\x00\xF0\x00"
        "\xC3\x00\xC3\x00\xC3\x00\xC3\x00\xC3\x00\xC3\x00\xFF\x00\xFF\x00"
        "\xCF\x00\xCF\x00\xCF\x00\xCF\x00\xCF\x00\xCF\x00\xC3\x00\xC3\x00"
        "\x0F\x00\x0F\x00\x0F\x00\x0F\x00\x0F\x00\x0F\x00\xFC\x00\xFC\x00"
        "\x3C\x00\x42\x00\xB9\x00\xA5\x00\xB9\x00\xA5\x00\x42\x00\x3C\x00";

    memcpy(&memory[0x8000], bg_data, sizeof(bg_data));

    uint8_t bg_map_data[] = "\x00\x00\x00\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C"
        "\x19\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

    memcpy(&memory[0x9800], bg_map_data, sizeof(bg_map_data));

    memory[eRegSCY] = 0;
    memory[eRegSCX] = 0;
    memory[eRegLCDC] = 0x91;

    for (int i = 0; i < 456 * 144; i++)
        state->display->UpdateTimer(4);

    SDL_Delay(10000);
}*/