#include <SDL2/SDL.h>
#include <unistd.h>

#include "Display.h"
#include "Memory.h"

const uint TILE_DATA_SIZE = 16; // Tile is 16 bytes.
const uint TILE_PIXEL_SIZE = 8; // Tile is 8x8 pixels.

const uint REAL_SCREEN_SIZE = 256;
const uint REAL_TILES_PER_SCREEN = REAL_SCREEN_SIZE / TILE_PIXEL_SIZE;
const uint TILES_PER_X = SCREEN_X / TILE_PIXEL_SIZE;
const uint TILES_PER_Y = SCREEN_Y / TILE_PIXEL_SIZE;
const uint BG_SIZE = 256;
const uint SCREEN_SCALE = 4;

const uint16_t BG_TILE_MAP[] = {0x9800, 0x9C00};
const uint16_t BG_TILE_MAP_LEN = 0x03FF;
const uint16_t BG_DATA_OFFSET[] = {0x8800, 0x8000};
const uint16_t BG_DATA_LEN = 0x0FFF;
const uint16_t WIN_TILE_MAP[] = {0x9800, 0x9C00};
const uint16_t WIN_TILE_MAP_LEN = 0x03FF;
const uint16_t OAM_RAM = 0xFE00;
const uint16_t OAM_LEN = 0x009F;
const uint16_t ClockPerScanline = 456;

const uint8_t palette[4][3] = {{0xFF, 0xFF, 0xFF}, {0xB0, 0xB0, 0xB0}, {0x68, 0x68, 0x68}, {0x00, 0x00, 0x00}};

enum LCDCBits
{
    eLCDCBGEnabled = 0x01,
    eLCDCSpriteEnabled = 0x02,
    eLCDCSpriteSize = 0x04,
    eLCDCBGTileMap = 0x08,
    eLCDCWindowTileset = 0x10,
    eLCDCWindowEnable = 0x20,
    eLCDCWindowTileMap = 0x40,
    eLCDCPower = 0x80
};

enum LCDStatBits
{
    eLCDStatLYLYCCompare = 0x04,
    eLCDStatMode0HBlank = 0x08,
    eLCDStatMode1Vblank = 0x10,
    eLCDStatMode2OAM = 0x20,
    eLCDStatLYLCCheck = 0x40
};

enum SpriteAttrBits
{
    eSpriteAttrPalette = 0x10,
    eSpriteAttrFlipX = 0x20,
    eSpriteAttrFlipY = 0x40,
    eSpriteAttrBgPriority = 0x80
};

Display::Display(std::shared_ptr<Memory> memory, std::shared_ptr<Interrupt> interrupts) :
    memory(memory),
    interrupts(interrupts),
    regLCDC(memory->GetBytePtr(eRegLCDC)),
    regSTAT(memory->GetBytePtr(eRegSTAT)),
    regSCY(memory->GetBytePtr(eRegSCY)),
    regSCX(memory->GetBytePtr(eRegSCX)),
    regLY(memory->GetBytePtr(eRegLY)),
    regLYC(memory->GetBytePtr(eRegLYC)),
    regDMA(memory->GetBytePtr(eRegDMA)),
    regBGP(memory->GetBytePtr(eRegBGP)),
    regOBP0(memory->GetBytePtr(eRegOBP0)),
    regOBP1(memory->GetBytePtr(eRegOBP1)),
    regWY(memory->GetBytePtr(eRegWY)),
    regWX(memory->GetBytePtr(eRegWX)),
    displayMode(eMode0HBlank),
    sdlWindow(NULL),
    sdlRenderer(NULL),
    counter(0)
{
    DBG("SDL Display\n");
    sdlWindow = SDL_CreateWindow("gbemu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_X*SCREEN_SCALE, SCREEN_Y*SCREEN_SCALE, SDL_WINDOW_SHOWN);
    if (!sdlWindow)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_SOFTWARE);
    SDL_RenderSetLogicalSize(sdlRenderer, SCREEN_X, SCREEN_Y);

    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_X, SCREEN_Y);
}


Display::~Display()
{
    if (timerSubject)
        timerSubject->DetachObserver(this);

    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    DBG("SDL_Destroy\n");
}


void Display::AttachToTimerSubject(std::shared_ptr<TimerSubject> subject)
{
    this->timerSubject = subject;
    subject->AttachObserver(shared_from_this());
}


void Display::UpdateTimer(uint value)
{
    // Clear counter and do nothing if LCD is off.
    if ((*regLCDC & eLCDCPower) == 0)
    {
        counter = 0;
        *regLY = 0;
        SetMode(eMode0HBlank);
        return;
    }

    bool oldStatCheck = GetStatCheck();

    // Set display mode bits. This is probably wrong. TCAGBD and GBCPUman have very different values for when the mode switches.
    if (*regLY >= 144)
    {
        if (counter == 0)
            SetMode(eMode0HBlank);
        else
            SetMode(eMode1VBlank);
    }
    else if (counter == 0 || counter >= 448)
    {
        SetMode(eMode0HBlank);
    }
    else if (counter >= 4 && counter <= 80)
    {
        SetMode(eMode2SearchingOAM);
    }
    else if (counter >= 84)
    {
        SetMode(eMode3TranferData);
    }

    // Increase counter
    counter += value;

    // Draw scanline when counter reaches an HBlank.
    if (counter >= ClockPerScanline)
        UpdateScanline();

    bool newStatCheck = GetStatCheck();

    // Trigger Stat interrupt only when check goes from 0 to 1.
    if (!oldStatCheck && newStatCheck)
        if (interrupts)
            interrupts->RequestInterrupt(eIntLCDC);
}


void Display::SetMode(DisplayModes mode)
{
    displayMode = mode;

    // Clear mode bits.
    *regSTAT &= 0xFC;
    *regSTAT |= mode;
}


void Display::UpdateScanline()
{
    //DBG("LY=%u\n", *regLY);
    
    counter = 0;

    if (*regLY < 144)
        DrawScanline(*regLY);

    *regLY = (*regLY + 1) % 154;

    if (*regLY == 0)
    {
        if (*regLCDC & eLCDCSpriteEnabled)
            DrawSprites();
        DrawScreen();
        //printf("SCY=%u\n", *regSCY);
        //usleep(16700);
        usleep(100);
        //SDL_Delay(17);
    }
    else if (*regLY == 144)
        interrupts->RequestInterrupt(eIntVBlank);

    // LY==LYC bit gets updated regardless of LY==LYC check bit.
    if (*regLY == *regLYC)
    {
        *regSTAT |= eLCDStatLYLYCCompare;
    }
    else
    {
        *regSTAT &= ~eLCDStatLYLYCCompare;
    }
}


bool Display::GetStatCheck()
{
    if ((*regLY == *regLYC) && (*regSTAT & eLCDStatLYLCCheck))
        return true;
    if ((displayMode == eMode0HBlank) && (*regSTAT & eLCDStatMode0HBlank))
        return true;
    if ((displayMode == eMode1VBlank) && ((*regSTAT & eLCDStatMode1Vblank) || (*regSTAT & eLCDStatMode2OAM)))
        return true;
    if ((displayMode == eMode2SearchingOAM) && (*regSTAT & eLCDStatMode2OAM))
        return true;

    return false;
}


void Display::DrawScanline(uint scanline)
{
    if (*regLCDC & eLCDCBGEnabled)
        DrawBackgroundScanline(scanline, *regSCX, *regSCY);
    if (*regLCDC & eLCDCWindowEnable)
        DrawWindowScanline(scanline, *regWX, *regWY);
    // if (*regLCDC & eLCDCSpriteEnabled)
    //     DrawSprites(scanline, *regSCX, *regSCY);
}


void Display::DrawBackgroundScanline(uint scanline, uint scrollX, uint scrollY)
{
    uint16_t tileDataOffset = (*regLCDC & eLCDCWindowTileset) ? BG_DATA_OFFSET[1] : BG_DATA_OFFSET[0];
    uint8_t *tileData = memory->GetBytePtr(tileDataOffset);
    uint16_t tileMapOffset = (*regLCDC & eLCDCBGTileMap) ? BG_TILE_MAP[1] : BG_TILE_MAP[0];
    uint8_t *tileMap = memory->GetBytePtr(tileMapOffset);

    for (uint i = 0; i < TILES_PER_X; i++)
    {
        uint8_t tileX = ((scrollX / TILE_PIXEL_SIZE) + i) % REAL_TILES_PER_SCREEN;
        uint8_t tileY = ((scrollY / TILE_PIXEL_SIZE) + (scanline / TILE_PIXEL_SIZE)) % REAL_TILES_PER_SCREEN;

        uint8_t tileId = tileMap[tileX + (tileY * REAL_TILES_PER_SCREEN)];
        if (tileDataOffset == BG_DATA_OFFSET[0])
        {
            // When BG data is 0x8800, tild id is a signed byte with 0 in the middle of the range.
            tileId = (int8_t)tileId + 0x80;
        }
        uint16_t tileOffset = (tileId * TILE_DATA_SIZE) + ((scanline % TILE_PIXEL_SIZE) * 2);

        DrawTileLine(tileData[tileOffset], tileData[tileOffset + 1], TILE_PIXEL_SIZE * i, scanline, *regBGP, false, false, true);
    }
}


void Display::DrawWindowScanline(uint scanline, uint windowX, uint windowY)
{
    // Return if current scanline is above the window.
    if (scanline < windowY)
        return;

    uint16_t tileDataOffset = (*regLCDC & eLCDCWindowTileset) ? BG_DATA_OFFSET[1] : BG_DATA_OFFSET[0];
    uint8_t *tileData = memory->GetBytePtr(tileDataOffset);
    uint16_t tileMapOffset = (*regLCDC & eLCDCWindowTileMap) ? BG_TILE_MAP[1] : BG_TILE_MAP[0];
    uint8_t *tileMap = memory->GetBytePtr(tileMapOffset);

    for (uint i = 0; i < TILES_PER_X; i++)
    {
        uint8_t tileX = i;
        uint8_t tileY = (scanline - windowY) / TILE_PIXEL_SIZE;

        if (((tileX * TILE_PIXEL_SIZE) + windowX) > SCREEN_X)
            return;

        uint8_t tileId = tileMap[tileX + (tileY * REAL_TILES_PER_SCREEN)];
        if (tileDataOffset == BG_DATA_OFFSET[0])
        {
            // When BG data is 0x8800, tild id is a signed byte with 0 in the middle of the range.
            tileId = (int8_t)tileId + 0x80;
        }
        uint16_t tileOffset = (tileId * TILE_DATA_SIZE) + ((scanline % TILE_PIXEL_SIZE) * 2);

        uint8_t xPos = (windowX - 7) + (TILE_PIXEL_SIZE * i);

        DrawTileLine(tileData[tileOffset], tileData[tileOffset + 1], xPos, scanline, *regBGP, false, false, true);
    }
}


//void Display::DrawSprites(uint scanline, uint scrollX, uint scrollY)
void Display::DrawSprites()
{
    //y, x, chr_code, attr_data
    const uint8_t spriteSize = (*regLCDC & eLCDCSpriteSize) ? 16 : 8;
    const uint8_t *oamRam = memory->GetBytePtr(OAM_RAM);
    const uint8_t *spriteData = memory->GetBytePtr(BG_DATA_OFFSET[1]);

    for (uint i = 0; i < 160; i += 4)
    {
        uint8_t yPos = oamRam[i + 0];
        uint8_t xPos = oamRam[i + 1];
        uint8_t spriteId = oamRam[i + 2];
        uint8_t spriteAttr = oamRam[i + 3];

        uint8_t paletteReg = (spriteAttr & eSpriteAttrPalette) ? *regOBP1 : *regOBP0;
        bool flipX = spriteAttr & eSpriteAttrFlipX;
        bool flipY = spriteAttr & eSpriteAttrFlipY;
        bool bgPriority = spriteAttr & eSpriteAttrBgPriority;

        // Sprites with a position of 0 are not displayed.
        if (xPos == 0 || yPos == 0)
            continue;

        // Sprite position needs to be offset because of 0 not being displayed.
        xPos -= 8;
        yPos -= 16;

        for (uint line_ = 0; line_ < spriteSize; line_++)
        {
            uint line = line_;
            if (flipY)
                line = (spriteSize - 1) - line_;

            uint16_t spriteOffset = (spriteId * spriteSize * 2) + (line * 2);
            DrawTileLine(spriteData[spriteOffset], spriteData[spriteOffset + 1], xPos, yPos + line_, paletteReg, flipX, bgPriority, false);
        }
    }
}


void Display::DrawTileLine(uint8_t byte1, uint8_t byte2, uint8_t xPos, uint8_t yPos, uint8_t paletteReg, bool flipX, bool bgPriority, bool isBg)
{
    if (yPos > SCREEN_Y)
        return;

    for (uint x_ = 0; x_ < TILE_PIXEL_SIZE; x_++)
    {
        if ((xPos + x_) > SCREEN_X)
            return;

        uint x = x_;
        if (flipX)
            x = (TILE_PIXEL_SIZE - 1) - x_;

        // Each tile is 16 bytes for 8x8 pixels. There are two bits per pixel, and each
        // pixel is split across two bytes. The pixel at x,y=0,0 is the most significant
        // bit of byte one and byte two. The pixel at x,y=1,0 is the second most significant
        // bits of byte one and two, etc.
        uint lowBit = ((byte1 >> (7-x)) & 0x1);
        uint highBit = ((byte2 >> (7-x)) & 0x1);
        uint pixelVal = lowBit | (highBit << 1);
        const uint8_t *color = palette[(paletteReg >> (pixelVal * 2)) & 0x03];

        uint pixelOffset = (yPos * SCREEN_X) + xPos + x_;

        if (isBg)
        {
            // Save the 2-bit color to use for sprite/BG priority.
            bgColorMap[pixelOffset] = pixelVal;
        }
        else
        {
            // Sprite color 0 is transparent.
            if (pixelVal == 0)
                continue;
        }

        // Set pixel color.
        if (isBg || // Background or window
            (!isBg && !bgPriority) || // Sprite, without BG priority
            (!isBg && bgPriority && bgColorMap[pixelOffset] == 0)) // Sprite, with BG priority, and BG color is 0
        {
            frameBuffer[pixelOffset] = (color[0] << 16) | (color[1] << 8) | color[2];
        }
    }
}


void Display::DrawScreen()
{
    SDL_UpdateTexture(sdlTexture, NULL, frameBuffer, SCREEN_X * 4);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(sdlRenderer);
}