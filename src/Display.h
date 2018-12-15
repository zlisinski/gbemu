#pragma once

#include "gbemu.h"
#include "Interrupt.h"
#include "TimerObserver.h"
#include "TimerSubject.h"

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_Texture;
struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;
class Memory;

const uint SCREEN_X = 160;
const uint SCREEN_Y = 144;

class Display : public TimerObserver, public std::enable_shared_from_this<Display>
{
public:
    Display(std::shared_ptr<Memory> memory, std::shared_ptr<Interrupt> interrupts);
    ~Display();

    void SetWindowTitle(const char *title);

    // Inherited from TimerObserver.
    virtual void AttachToTimerSubject(std::shared_ptr<TimerSubject> subject);
    virtual void UpdateTimer(uint value);

private:
    enum DisplayModes
    {
        eMode0HBlank = 0,
        eMode1VBlank = 1,
        eMode2SearchingOAM = 2,
        eMode3TranferData = 3,
    };

    void SetMode(DisplayModes mode);
    void UpdateScanline();

    bool GetStatCheck();

    void DrawScanline(uint scanline);
    void DrawBackgroundScanline(uint scanline, uint scrollX, uint scrollY);
    void DrawWindowScanline(uint scanline, uint windowX, uint windowY);
    void DrawSprites();
    void DrawTileLine(uint8_t byte1, uint8_t byte2, uint8_t xPos, uint8_t yPos, uint8_t paletteReg, bool flipX, bool bgPriority, bool isBg);

    void DrawScreen();
    void DrawFPS();

    std::shared_ptr<Memory> memory;
    std::shared_ptr<Interrupt> interrupts;

    uint8_t *regLCDC;  // 0xFF40 LCD control
    uint8_t *regSTAT;  // 0xFF41 LCDC status
    uint8_t *regSCY;   // 0xFF42 Scroll Y
    uint8_t *regSCX;   // 0xFF43 Scroll X
    uint8_t *regLY;    // 0xFF44 LCDC Y coordinate
    uint8_t *regLYC;   // 0xFF45 LY compare
    uint8_t *regDMA;   // 0xFF46 DMA transfer start address
    uint8_t *regBGP;   // 0xFF47 BG & window palette data
    uint8_t *regOBP0;  // 0xFF48 Object palette 0 data
    uint8_t *regOBP1;  // 0xFF49 Object palette 1 data
    uint8_t *regWY;    // 0xFF4A Window Y position
    uint8_t *regWX;    // 0xFF4B Window X position

    DisplayModes displayMode;

    SDL_Window *sdlWindow;
    SDL_Renderer *sdlRenderer;
    SDL_Texture *sdlTexture;
    SDL_Texture *sdlFpsTexture;
    TTF_Font *sdlFont;

    uint32_t frameBuffer[SCREEN_X * SCREEN_Y];
    uint8_t bgColorMap[SCREEN_X * SCREEN_Y];

    uint16_t counter;

    uint32_t frameTicks;
    uint32_t frameCount;
};