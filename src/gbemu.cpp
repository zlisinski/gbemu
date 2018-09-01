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


bool loadROM(const char *filename, uint8_t **memory)
{
    *memory = NULL;

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening %s\n", filename);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
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


int main()
{
    State state;
    uint8_t *romMemory = NULL;
    Emulator emulator(&state);

    if (!LoadBIOS("data/BIOS.gb", state.memory.GetBytePtr(0)))
    {
        exit(1);
    }

    if (!loadROM("data/Tetris.gb", &romMemory))
    {
        exit(1);
    }

    while (true)
    {
        emulator.ProcessOpCode();
    }

    delete [] romMemory;

    return 0;
}