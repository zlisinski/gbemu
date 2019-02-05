#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>
#include <sstream>

#include "gbemu.h"
#include "Emulator.h"
#include "Globals.h"
#include "Input.h"
#include "State.h"


bool runbootRom = false;
bool debugOutput = false;

const std::map<SDL_Keycode, Buttons::Button> keymap = {
    {SDLK_w, Buttons::Button::eButtonUp},
    {SDLK_s, Buttons::Button::eButtonDown},
    {SDLK_a, Buttons::Button::eButtonLeft},
    {SDLK_d, Buttons::Button::eButtonRight},
    {SDLK_h, Buttons::Button::eButtonSelect},
    {SDLK_j, Buttons::Button::eButtonStart},
    {SDLK_k, Buttons::Button::eButtonB},
    {SDLK_l, Buttons::Button::eButtonA}
};
const std::map<uint8_t, Buttons::Button> joymap = {
    {6, Buttons::Button::eButtonSelect},
    {7, Buttons::Button::eButtonStart},
    {2, Buttons::Button::eButtonB},
    {0, Buttons::Button::eButtonA}
};


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


void ProcessInput(const SDL_Event &e, Buttons &buttons, Input* input)
{
    uint8_t oldButtonData = buttons.data;

    if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
    {
        auto it = keymap.find(e.key.keysym.sym);
        if (it != keymap.end())
        {
            if (e.type == SDL_KEYDOWN)
                buttons.data |= it->second;
            else
                buttons.data &= ~it->second;
        }
    }
    else if (e.type == SDL_JOYBUTTONDOWN || e.type == SDL_JOYBUTTONUP)
    {
        auto it = joymap.find(e.jbutton.button);
        if (it != joymap.end())
        {
            if (e.type == SDL_JOYBUTTONDOWN)
                buttons.data |= it->second;
            else
                buttons.data &= ~it->second;
        }
    }
    else if (e.type == SDL_JOYHATMOTION)
    {
        buttons.data = (buttons.data & 0xF0) | (e.jhat.value & 0x0F);
    }

    if (buttons.data != oldButtonData)
        input->SetButtons(buttons);
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

    State* state = new State();
    if (runbootRom)
        state->SetRomMemory(bootRomMemory, gameRomMemory);
    else
        state->SetRomMemory(gameRomMemory);
    Emulator emulator(state);

    bool quit = false;
    bool pause = false;
    SDL_Event e;
    SDL_Joystick *joystick = NULL;
    Buttons buttons;

    std::stringstream ss;
    ss << "gbemu " << romFilename;
    std::string title = ss.str();
    state->display->SetWindowTitle(title.c_str());

    if (SDL_NumJoysticks() >= 1)
    {
        joystick = SDL_JoystickOpen(0);
        if (joystick == NULL)
            printf("Cant open joystick\n");
        else
            printf("Opened joystick\n");
    }
    else
        printf("No joysticks found\n");

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            {
                pause = !pause;
                state->display->SetWindowTitle(pause ? (title + " Paused").c_str() : title.c_str());
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F10)
            {
                Globals::getInstance().showFps = !Globals::getInstance().showFps;
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F11)
            {
                Globals::getInstance().capFps = !Globals::getInstance().capFps;
            }
            else
                ProcessInput(e, buttons, state->input);
        }

        // Process 1000 instructions before polling for events.
        for (int i = 0; i < 1000 && !pause; i++)
        {
            emulator.ProcessOpCode();
            state->PrintState();
            //state.timer->PrintTimerData();
            DBG("\n");
        }
    }

    /*uint8_t *mem = state->memory->GetBytePtr(0);

    printf("\n");
    HexDump(mem, 0xFF00, 0x48);
    printf("\n");
    HexDump(mem, 0x8000, 0x1800);
    printf("\n");
    HexDump(mem, 0x9800, 0x0400);
    printf("\n");
    HexDump(mem, 0x9C00, 0x0400);*/

    delete state;

    if (joystick)
        SDL_JoystickClose(joystick);

    TTF_Quit();
    SDL_Quit();

    return 0;
}