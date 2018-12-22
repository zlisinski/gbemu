#include <gtest/gtest.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "EmulatorTest.h"

bool debugOutput = false;

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    if (TTF_Init() != 0)
    {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        exit(1);
    }

    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    TTF_Quit();
    SDL_Quit();

    return ret;
}