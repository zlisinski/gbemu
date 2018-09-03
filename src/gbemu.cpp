#include <string.h>

#include "gbemu.h"
#include "State.h"
#include "Emulator.h"

const size_t BIOS_SIZE = 0xFF;
const size_t ROM_BANK_SIZE = 0x4000;


bool LoadBIOS(const char *filename, uint8_t *memory)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening %s\n", filename);
        return false;
    }

    size_t cnt = fread(memory, 1, BIOS_SIZE, file);
    if (cnt != BIOS_SIZE)
    {
        printf("Only read %ld bytes from %s", cnt, filename);
        return false;
    }

    return true;
}


bool loadROM(const char *filename, uint8_t **memory, long &size)
{
    *memory = NULL;
    size = 0;

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening %s\n", filename);
        return false;
    }

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *memory = new uint8_t[size]();

    size_t cnt = fread(*memory, 1, size, file);
    if (cnt != (size_t)size)
    {
        printf("Only read %lu bytes of %ld from %s", cnt, size, filename);
        return false;
    }

    return true;
}


int main(int argc, char **argv)
{
    State state;
    uint8_t *romMemory = NULL;
    long romSize = 0;
    Emulator emulator(&state);

    if (argc < 2)
    {
        printf("Usage: %s rom_filename\n", argv[0]);
        exit(1);
    }

    const char *romFilename = argv[1];

    /*if (!LoadBIOS("data/BIOS.gb", state.memory.GetBytePtr(0)))
    {
        exit(1);
    }*/

    if (!loadROM(romFilename, &romMemory, romSize))
    {
        exit(1);
    }

    memcpy(state.memory.GetBytePtr(0), romMemory, romSize);

    // Set state to what it would be after running the BIOS.
    state.a = 0x01;
    state.f = 0xB0;
    state.bc = 0x0013;
    state.de = 0x00D8;
    state.hl = 0x014D;
    state.sp = 0xFFFE;
    state.memory.WriteByte(eRegTIMA, 0x00);
    state.memory.WriteByte(eRegTMA, 0x00);
    state.memory.WriteByte(eRegTAC, 0x00);
    state.memory.WriteByte(eRegNR10, 0x80);
    state.memory.WriteByte(eRegNR11, 0xBF);
    state.memory.WriteByte(eRegNR12, 0xF3);
    state.memory.WriteByte(eRegNR14, 0xBF);
    state.memory.WriteByte(eRegNR21, 0x3F);
    state.memory.WriteByte(eRegNR22, 0x00);
    state.memory.WriteByte(eRegNR24, 0xBF);
    state.memory.WriteByte(eRegNR30, 0x7F);
    state.memory.WriteByte(eRegNR31, 0xFF);
    state.memory.WriteByte(eRegNR32, 0x9F);
    state.memory.WriteByte(eRegNR34, 0xBF);
    state.memory.WriteByte(eRegNR41, 0xFF);
    state.memory.WriteByte(eRegNR42, 0x00);
    state.memory.WriteByte(eRegNR43, 0x00);
    state.memory.WriteByte(eRegNR44, 0xBF);
    state.memory.WriteByte(eRegNR50, 0x77);
    state.memory.WriteByte(eRegNR51, 0xF3);
    state.memory.WriteByte(eRegNR52, 0xF1);
    state.memory.WriteByte(eRegLCDC, 0x91);
    state.memory.WriteByte(eRegSCY, 0x00);
    state.memory.WriteByte(eRegSCX, 0x00);
    state.memory.WriteByte(eRegLYC, 0x00);
    state.memory.WriteByte(eRegBGP, 0xFC);
    state.memory.WriteByte(eRegOBP0, 0xFF);
    state.memory.WriteByte(eRegOBP1, 0xFF);
    state.memory.WriteByte(eRegWY, 0x00);
    state.memory.WriteByte(eRegWX, 0x00);
    state.memory.WriteByte(eRegIE, 0x00);

    state.pc = 0x0100;

    while (true)
    {
        emulator.ProcessOpCode();
    }

    delete [] romMemory;

    return 0;
}