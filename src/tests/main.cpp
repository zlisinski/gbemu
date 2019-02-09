#include <gtest/gtest.h>
#include <SDL2/SDL.h>
#include "main.h"
#include "../Display.h"

bool debugOutput = false;

SDL_Window *sdlWindow = NULL;
SDL_Renderer *sdlRenderer = NULL;
SDL_Texture *sdlTexture = NULL;
uint32_t lastFrameTicks = 0;


void SetupWindow()
{
    sdlWindow = SDL_CreateWindow("gbemu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 720, SDL_WINDOW_SHOWN);
    if (!sdlWindow)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(sdlRenderer, SCREEN_X, SCREEN_Y);

    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_X, SCREEN_Y);
}


void DrawFrame(uint32_t *frameBuffer)
{
    if (sdlWindow == NULL)
        SetupWindow();

    SDL_UpdateTexture(sdlTexture, NULL, frameBuffer, SCREEN_X * 4);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(sdlRenderer);

    uint32_t ticks = SDL_GetTicks();

    // Cap frame rate.
    const float frameMillis = 1.0 / 60 * 1000;
    if ((ticks - lastFrameTicks) < frameMillis)
    {
        usleep((frameMillis - (ticks - lastFrameTicks)) * 1000);
    }

    lastFrameTicks = ticks;
}


int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);

    SDL_Quit();

    return ret;
}