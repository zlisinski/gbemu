#include <algorithm>
#include <memory>
#include <sstream>
#include <unistd.h>

#include "Display.h"
#include "DisplayInterface.h"
#include "Globals.h"
#include "Logger.h"
#include "Memory.h"

const uint TILE_DATA_SIZE = 16; // Tile is 16 bytes.
const uint TILE_PIXEL_SIZE = 8; // Tile is 8x8 pixels.

const uint REAL_SCREEN_SIZE = 256;
const uint REAL_TILES_PER_SCREEN = REAL_SCREEN_SIZE / TILE_PIXEL_SIZE;

const uint16_t BG_TILE_MAP[] = {0x9800, 0x9C00};
const uint16_t BG_DATA_OFFSET[] = {0x8800, 0x8000};
const uint16_t OAM_RAM = 0xFE00;
const uint16_t OAM_LEN = 0x00A0;
const uint16_t ClockPerScanline = 456;
const uint8_t MAX_SPRITES_PER_SCANLINE = 10;

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


Display::Display(Memory *memory, Interrupt *interrupts, DisplayInterface *displayInterface, TimerSubject *timerSubject) :
    memory(memory),
    interrupts(interrupts),
    regLCDC(memory->AttachIoRegister(eRegLCDC, this)),
    regSTAT(memory->AttachIoRegister(eRegSTAT, this)),
    regSCY(memory->AttachIoRegister(eRegSCY, this)),
    regSCX(memory->AttachIoRegister(eRegSCX, this)),
    regLY(memory->AttachIoRegister(eRegLY, this)),
    regLYC(memory->AttachIoRegister(eRegLYC, this)),
    regBGP(memory->AttachIoRegister(eRegBGP, this)),
    regOBP0(memory->AttachIoRegister(eRegOBP0, this)),
    regOBP1(memory->AttachIoRegister(eRegOBP1, this)),
    regWY(memory->AttachIoRegister(eRegWY, this)),
    regWX(memory->AttachIoRegister(eRegWX, this)),
    displayMode(eMode0HBlank),
    counter(0),
    displayInterface(displayInterface)
{
    timerSubject->AttachObserver(this);
}


Display::~Display()
{

}


bool Display::SaveState(FILE *file)
{
    if (!fwrite(&displayMode, sizeof(displayMode), 1, file))
        return false;

    if (!fwrite(&counter, sizeof(counter), 1, file))
        return false;

    return true;
}


bool Display::LoadState(uint16_t version, FILE *file)
{
    (void)version;

    if (!fread(&displayMode, sizeof(displayMode), 1, file))
        return false;

    if (!fread(&counter, sizeof(counter), 1, file))
        return false;

    // Update framebuffer so the first frame isn't garbage.
    for (uint i = 0; i < SCREEN_Y; i++)
        DrawScanline(i);

    return true;
}


bool Display::WriteByte(uint16_t address, uint8_t byte)
{
    LogInstruction("Display::WriteByte %04X, %02X", address, byte);

    switch (address)
    {
        case eRegLCDC:
            *regLCDC = byte;
            return true;
        case eRegSTAT:
            // Preserve lower 3 read-only bits, top bit is always set.
            *regSTAT = (byte & 0xFC) | (*regSTAT & 0x03) | 0x80;
            return true;
        case eRegSCY:
            *regSCY = byte;
            return true;
        case eRegSCX:
            *regSCX = byte;
            return true;
        case eRegLY:
            //regLY is read-only.
            return true;
        case eRegLYC:
            *regLYC = byte;
            return true;
        case eRegBGP:
            *regBGP = byte;
            return true;
        case eRegOBP0:
            *regOBP0 = byte;
            return true;
        case eRegOBP1:
            *regOBP1 = byte;
            return true;
        case eRegWY:
            *regWY = byte;
            return true;
        case eRegWX:
            *regWX = byte;
            return true;
        default:
            return false;
    }
}


uint8_t Display::ReadByte(uint16_t address) const
{
    LogInstruction("Display::ReadByte %04X", address);

    switch (address)
    {
        case eRegLCDC:
            return *regLCDC;
        case eRegSTAT:
            // Unused bit is always 1.
            return *regSTAT | 0x80;
        case eRegSCY:
            return *regSCY;
        case eRegSCX:
            return *regSCX;
        case eRegLY:
            return *regLY;
        case eRegLYC:
            return *regLYC;
        case eRegBGP:
            return *regBGP;
        case eRegOBP0:
            return *regOBP0;
        case eRegOBP1:
            return *regOBP1;
        case eRegWY:
            return *regWY;
        case eRegWX:
            return *regWX;
        default:
            std::stringstream ss;
            ss << "Display doesnt handle reads to 0x" << std::hex << std::setw(4) << std::setfill('0') << address;
            throw std::range_error(ss.str());
    }
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
        DrawScreen();
        //printf("SCY=%u\n", *regSCY);
        //usleep(16700);
        //usleep(100);
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


void Display::DrawScanline(uint8_t scanline)
{
    if (*regLCDC & eLCDCBGEnabled)
        DrawBackgroundScanline(scanline, *regSCX, *regSCY);
    if (*regLCDC & eLCDCWindowEnable)
        DrawWindowScanline(scanline, *regWX, *regWY);
    if (*regLCDC & eLCDCSpriteEnabled)
        DrawSprites(scanline);
}


void Display::DrawBackgroundScanline(uint8_t scanline, uint8_t scrollX, uint8_t scrollY)
{
    const uint16_t tilesetDataOffset = (*regLCDC & eLCDCWindowTileset) ? BG_DATA_OFFSET[1] : BG_DATA_OFFSET[0];
    const uint8_t * const tilesetData = memory->GetBytePtr(tilesetDataOffset);
    const uint16_t tileMapOffset = (*regLCDC & eLCDCBGTileMap) ? BG_TILE_MAP[1] : BG_TILE_MAP[0];
    const uint8_t * const tileMap = memory->GetBytePtr(tileMapOffset);

    const uint y = scanline + scrollY;
    const uint8_t tileY = (y / TILE_PIXEL_SIZE) & (REAL_TILES_PER_SCREEN - 1);

    for (uint i = 0; i < SCREEN_X; i++)
    {
        const uint x = i + scrollX;
        const uint8_t tileX = (x / TILE_PIXEL_SIZE) & (REAL_TILES_PER_SCREEN - 1);

        uint8_t tileId = tileMap[tileX + (tileY * REAL_TILES_PER_SCREEN)];
        if (tilesetDataOffset == BG_DATA_OFFSET[0])
        {
            // When BG data is 0x8800, tile id is a signed byte with 0 in the middle of the range.
            tileId = (int8_t)tileId + 0x80;
        }

        const uint16_t tileDataOffset = (tileId * TILE_DATA_SIZE) + ((y & 7) * 2);
        const uint8_t *tileData = &tilesetData[tileDataOffset];
        const uint8_t lowBit = ((tileData[0] >> (7 - (x & 7))) & 0x01);
        const uint8_t highBit = ((tileData[1] >> (7 - (x & 7))) & 0x01);
        const uint8_t pixelVal = lowBit | (highBit << 1);
        const uint8_t * const color = palette[(*regBGP >> (pixelVal * 2)) & 0x03];
        const uint16_t pixelOffset = (scanline * SCREEN_X) + i;

        // Save palette index for this pixel to use later for sprite priority.
        bgColorMap[pixelOffset] = pixelVal;

        frameBuffer[pixelOffset] = (color[0] << 16) | (color[1] << 8) | color[2];
    }
}


void Display::DrawWindowScanline(uint8_t scanline, uint8_t windowX, uint8_t windowY)
{
    // TODO: Handle wx changing between vblanks.
    // Return if current scanline is above the window.
    if (scanline < windowY)
        return;

    // wx is offset by 7.
    int wx = windowX - 7;

    const uint16_t tilesetDataOffset = (*regLCDC & eLCDCWindowTileset) ? BG_DATA_OFFSET[1] : BG_DATA_OFFSET[0];
    const uint8_t * const tilesetData = memory->GetBytePtr(tilesetDataOffset);
    const uint16_t tileMapOffset = (*regLCDC & eLCDCWindowTileMap) ? BG_TILE_MAP[1] : BG_TILE_MAP[0];
    const uint8_t * const tileMap = memory->GetBytePtr(tileMapOffset);

    const uint y = scanline - windowY;
    const uint8_t tileY = (y / TILE_PIXEL_SIZE);

    for (uint i = std::max(0, wx); i < SCREEN_X; i++)
    {
        const uint x = i - wx;
        const uint8_t tileX = x / TILE_PIXEL_SIZE;

        uint8_t tileId = tileMap[tileX + (tileY * REAL_TILES_PER_SCREEN)];
        if (tilesetDataOffset == BG_DATA_OFFSET[0])
        {
            // When BG data is 0x8800, tile id is a signed byte with 0 in the middle of the range.
            tileId = (int8_t)tileId + 0x80;
        }

        const uint16_t tileDataOffset = (tileId * TILE_DATA_SIZE) + ((y & 7) * 2);
        const uint8_t *tileData = &tilesetData[tileDataOffset];
        const uint8_t lowBit = ((tileData[0] >> (7 - (x & 7))) & 0x01);
        const uint8_t highBit = ((tileData[1] >> (7 - (x & 7))) & 0x01);
        const uint8_t pixelVal = lowBit | (highBit << 1);
        const uint8_t * const color = palette[(*regBGP >> (pixelVal * 2)) & 0x03];
        const uint16_t pixelOffset = (scanline * SCREEN_X) + i;

        // Save palette index for this pixel to use later for sprite priority.
        bgColorMap[pixelOffset] = pixelVal;

        frameBuffer[pixelOffset] = (color[0] << 16) | (color[1] << 8) | color[2];
    }
}


void Display::DrawSprites(uint8_t scanline)
{
    const uint8_t xDisplayOffset = 8;
    const uint8_t yDisplayOffset = 16;
    const uint8_t spriteSize = (*regLCDC & eLCDCSpriteSize) ? 16 : 8;
    const uint8_t * const oamRam = memory->GetBytePtr(OAM_RAM);
    const uint8_t * const spriteData = memory->GetBytePtr(BG_DATA_OFFSET[1]);

    std::vector<SpriteData> sprites;
    sprites.reserve(MAX_SPRITES_PER_SCANLINE);

    uint8_t spriteCount = 0;

    // Find all sprites on scanline.
    for (uint8_t i = 0; i < OAM_LEN; i += 4)
    {
        const int16_t yPos = oamRam[i + 0] - yDisplayOffset;
        const int16_t xPos = oamRam[i + 1] - xDisplayOffset;

        // Add sprites that are on this scanline.
        if (scanline >= yPos && scanline < (yPos + spriteSize))
        {
            // GB can only display 10 sprites per scanline.
            spriteCount++;
            if (spriteCount > MAX_SPRITES_PER_SCANLINE)
                break;

            // Sprites with a 0 x position aren't displayed, but still count towards the number of sprites drawn.
            if (xPos + xDisplayOffset > 0)
                sprites.push_back({xPos, i});
        }
    }

    // Return if there are no sprites to draw on this scanline.
    if (sprites.size() == 0)
        return;

    // Sort sprites according to x and i priority.
    std::sort(sprites.begin(), sprites.end(), Display::SpriteSort);

    for (const SpriteData &sprite : sprites)
    {
        const uint8_t i = sprite.i;
        const int16_t yPos = oamRam[i + 0] - yDisplayOffset;
        const int16_t xPos = oamRam[i + 1] - xDisplayOffset;
        uint8_t spriteId = oamRam[i + 2];
        const uint8_t spriteAttr = oamRam[i + 3];
        const uint8_t paletteReg = (spriteAttr & eSpriteAttrPalette) ? *regOBP1 : *regOBP0;
        const bool flipX = spriteAttr & eSpriteAttrFlipX;
        const bool flipY = spriteAttr & eSpriteAttrFlipY;
        const bool bgPriority = spriteAttr & eSpriteAttrBgPriority;

        // In 8x16 mode, only even sprites can be used.
        if (spriteSize == 16)
            spriteId &= 0xFE;

        uint8_t line = (scanline - yPos); // TODO: is this right?
        if (flipY)
            line = (spriteSize - 1) - line;
        const uint16_t spriteDataOffset = (spriteId * TILE_DATA_SIZE) + (line * 2);
        const uint8_t * const tileData = &spriteData[spriteDataOffset];

        for (uint i = 0; i < TILE_PIXEL_SIZE; i++)
        {
            // xPos can be < 0 if the sprite is only partially on-screen.
            if (xPos + (int)i < 0)
                continue;

            uint x = i;
            if (flipX)
                x = (TILE_PIXEL_SIZE - 1) - i;
            const uint8_t lowBit = ((tileData[0] >> (7 - x)) & 0x01);
            const uint8_t highBit = ((tileData[1] >> (7 - x)) & 0x01);
            const uint8_t pixelVal = lowBit | (highBit << 1);

            // Sprite color 0 is transparent.
            if (pixelVal == 0)
                continue;

            const uint8_t * const color = palette[(paletteReg >> (pixelVal * 2)) & 0x03];
            const uint16_t pixelOffset = (scanline * SCREEN_X) + xPos + i;

            // Only draw pixel if background doesn't have priority, or background is 0.
            if (!bgPriority || (bgPriority && bgColorMap[pixelOffset] == 0))
                frameBuffer[pixelOffset] = (color[0] << 16) | (color[1] << 8) | color[2];
        }
    }
}


void Display::DrawScreen()
{
    displayInterface->FrameReady(frameBuffer);
}


bool Display::SpriteSort(const SpriteData &a, const SpriteData &b)
{
    if (a.x == b.x)
        return a.i > b.i;

    return a.x > b.x;
}