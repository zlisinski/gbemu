#include <SDL2/SDL.h>
#include <unistd.h>

#include "Display.h"
#include "Memory.h"

const uint TILE_DATA_SIZE = 16; // Tile is 16 bytes.
const uint TILE_PIXEL_SIZE = 8; // Tile is 8x8 pixels.

const uint SCREEN_X = 160;
const uint SCREEN_Y = 144;
const uint REAL_SCREEN_SIZE = 256;
const uint REAL_TILES_PER_SCREEN = REAL_SCREEN_SIZE / TILE_PIXEL_SIZE;
const uint TILES_PER_X = SCREEN_X / TILE_PIXEL_SIZE;
const uint TILES_PER_Y = SCREEN_Y / TILE_PIXEL_SIZE;
const uint BG_SIZE = 256;
const uint SCREEN_SCALE = 3;

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
    sdlWindow(NULL),
    sdlRenderer(NULL),
    frameBuffer(NULL),
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

    frameBuffer = new uint32_t[SCREEN_X * SCREEN_Y]();
}


Display::~Display()
{
    delete [] frameBuffer;

    if (subject)
        subject->DetachObserver(this);

    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    DBG("SDL_Destroy\n");
}


void Display::AttachToSubject(std::shared_ptr<TimerSubject> subject)
{
    this->subject = subject;
    subject->AttachObserver(shared_from_this());
}


void Display::UpdateTimer(uint value)
{
    // Clear counter and do nothing if LCD is off.
    if ((*regLCDC & eLCDCPower) == 0)
    {
        counter = 0;
        *regLY = 0;
        SetMode(eMode1VBlank);
        return;
    }

    // Set display mode bits. This is probably wrong. TCAGBD and GBCPUman have very different values for when the mode switches.
    if (*regLY >= 144)
    {
        if (counter == 0)
            SetMode(eMode0HBlank);
        else
            SetMode(eMode1VBlank);
    }
    if (counter == 0 || counter >= 448)
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
}


void Display::SetMode(DisplayModes mode)
{
    // Clear mode bits.
    *regSTAT &= 0xFC;

    *regSTAT |= mode;
}


void Display::UpdateScanline()
{
    //DBG("LY=%u\n", *regLY);
    
    counter = 0;

    if (*regLY < 144)
        DrawScanline(*regLY, *regSCX, *regSCY);

    *regLY = (*regLY + 1) % 154;

    if (*regLY == 0)
    {
        DrawSprites();
        DrawScreen();
        //printf("SCY=%u\n", *regSCY);
        //usleep(16700);
        usleep(100);
        //SDL_Delay(17);
    }
    else if (*regLY == 144)
        interrupts->RequestInterrupt(eIntVBlank);
}


void Display::DrawScanline(uint scanline, uint scrollX, uint scrollY)
{
    DrawBackground(scanline, scrollX, scrollY);
    //DrawSprites(scanline, scrollX, scrollY);
}


void Display::DrawBackground(uint scanline, uint scrollX, uint scrollY)
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

        DrawLine(tileData[tileOffset], tileData[tileOffset + 1], TILE_PIXEL_SIZE * i, scanline, *regBGP, false, true);
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

        // Sprites with a position of 0 are not displayed.
        if (xPos == 0 || yPos == 0)
            continue;

        // Sprite position needs to be offset because of 0 not being displayed.
        xPos -= 8;
        yPos -= 16;

        for (uint line = 0; line < spriteSize; line++)
        {
            uint16_t spriteOffset = (spriteId * spriteSize * 2) + (line * 2);
            DrawLine(spriteData[spriteOffset], spriteData[spriteOffset + 1], xPos, yPos + line, *regOBP0, false, true);
        }
    }
}


void Display::DrawLine(uint8_t byte1, uint8_t byte2, uint8_t xPos, uint8_t yPos, uint8_t paletteReg, bool flipX, bool priority)
{
    for (uint x = 0; x < TILE_PIXEL_SIZE; x++)
    {
        // Each tile is 16 bytes for 8x8 pixels. There are two bits per pixel, and each
        // pixel is split across two bytes. The pixel at x,y=0,0 is the most significant
        // bit of byte one and byte two. The pixel at x,y=1,0 is the second most significant
        // bits of byte one and two, etc.
        uint lowBit = ((byte1 >> (7-x)) & 0x1);
        uint highBit = ((byte2 >> (7-x)) & 0x1);
        uint pixelVal = lowBit | (highBit << 1);
        //const uint8_t *color = palette[(paletteReg >> (pixelVal * 2)) * 0x03];
        const uint8_t *color = palette[pixelVal];

        // Get a pointer to the pixel.
        uint32_t *pixel = &frameBuffer[(yPos * SCREEN_X) + xPos + x];

        // Set pixel color.
        *pixel = (color[0] << 16) | (color[1] << 8) | color[2];
    }
}


void Display::DrawScreen()
{
    SDL_UpdateTexture(sdlTexture, NULL, frameBuffer, SCREEN_X * 4);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(sdlRenderer);
}