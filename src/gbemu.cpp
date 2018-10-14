#include <SDL2/SDL.h>

#include "gbemu.h"
#include "State.h"
#include "Emulator.h"

bool runbootRom = false;
bool debugOutput = false;


void Usage(const char *mesage, const char *prog)
{
    printf("%s\n", mesage);
    printf("Usage: %s [-d] rom_filename\n", prog);
    printf("  -b        Run boot ROM\n");
    printf("  -d        Debug output\n");

    exit(1);
}


int ParseArgs(int argc, char **argv)
{
    int c;

    while ((c = getopt(argc, argv, "bd")) != -1)
    {
        switch (c)
        {
            case 'b':
                runbootRom = true;
                break;
            case 'd':
                debugOutput = true;
                break;
            default:
                std::string str("Unknown option: ");
                str += c;
                Usage(str.c_str(), argv[0]);
                break;
        }
    }

    return optind;
}


bool LoadBootRom(const char *filename, std::array<uint8_t, BOOT_ROM_SIZE> &memory)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening %s\n", filename);
        return false;
    }

    size_t cnt = fread(memory.data(), 1, BOOT_ROM_SIZE, file);
    if (cnt != BOOT_ROM_SIZE)
    {
        printf("Only read %ld bytes from %s", cnt, filename);
        return false;
    }

    return true;
}


bool loadGameRom(const char *filename, std::vector<uint8_t> &memory)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening %s\n", filename);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    memory.resize(size);

    size_t cnt = fread(&memory[0], 1, size, file);
    if (cnt != (size_t)size)
    {
        printf("Only read %lu bytes of %ld from %s", cnt, size, filename);
        return false;
    }

    return true;
}


void DumpMemory(uint8_t *mem, uint start, uint len)
{
    for (uint i = 0; i < len; i += 16)
    {
        uint off = start + i;
        uint8_t *b = mem + off;
        printf("%04X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", off,
            b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9], b[10], b[11], b[12], b[13], b[14], b[15]);
    }
}


int main(int argc, char **argv)
{
    int optind = ParseArgs(argc, argv);

    printf("debug=%d\n", debugOutput);
    printf("optind=%d\n", optind);
    printf("argv[optind]=%s\n", argv[optind]);
    //exit(0);

    if (!argv[optind])
    {
        Usage("Missing ROM filename", argv[0]);
    }

    const char *romFilename = argv[optind];

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    std::array<uint8_t, BOOT_ROM_SIZE> bootRomMemory;
    std::vector<uint8_t> gameRomMemory;

    if (!LoadBootRom("data/BIOS.gb", bootRomMemory))
    {
        exit(1);
    }

    if (!loadGameRom(romFilename, gameRomMemory))
    {
        exit(1);
    }

    State state;
    if (runbootRom)
        state.SetRomMemory(bootRomMemory, gameRomMemory);
    else
        state.SetRomMemory(gameRomMemory);
    Emulator emulator(&state);

    while (true)
    {
        emulator.ProcessOpCode();
        state.PrintState();
        //state.timer->PrintTimerData();
        DBG("\n");

        /*if (state.pc >= 0x100)
        {
            SDL_Delay(10000);
            break;
        }*/
    }

    /*uint8_t *mem = state.memory->GetBytePtr(0);

    printf("\n");
    DumpMemory(mem, 0xFF00, 0x48);
    printf("\n");
    DumpMemory(mem, 0x8000, 0x1800);
    printf("\n");
    DumpMemory(mem, 0x9800, 0x0400);
    printf("\n");
    DumpMemory(mem, 0x9C00, 0x0400);*/

    SDL_Quit();

    return 0;
}