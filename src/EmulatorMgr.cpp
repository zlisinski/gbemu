#include "gbemu.h"
#include "Emulator.h"
#include "EmulatorMgr.h"
#include "Input.h"
#include "State.h"


bool debugOutput = false;

EmulatorMgr::EmulatorMgr(void (*drawFrameCallback)(uint32_t *)) :
    paused(false),
    quit(false),
    drawFrameCallback(drawFrameCallback),
    buttons(),
    state(NULL)
{

}


EmulatorMgr::~EmulatorMgr()
{
    QuitEmulation();
}


bool EmulatorMgr::LoadRom(const char *filename)
{
    QuitEmulation();

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening %s\n", filename);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::vector<uint8_t> *memory = new std::vector<uint8_t>();
    memory->resize(size);

    size_t cnt = fread(&(*memory)[0], 1, size, file);
    if (cnt != (size_t)size)
    {
        printf("Only read %lu bytes of %ld from %s", cnt, size, filename);
        return false;
    }

    quit = false;
    workThread = std::thread(&EmulatorMgr::ThreadFunc, this, memory);

    return true;
}


void EmulatorMgr::PauseEmulation(bool pause)
{
    paused = pause;
}


void EmulatorMgr::QuitEmulation()
{
    if (workThread.joinable())
    {
        quit = true;
        workThread.join();
    }
}


void EmulatorMgr::ButtonPressed(Buttons::Button button)
{
    uint8_t oldButtonData = buttons.data;

    // Set bit for button.
    buttons.data |= button;

    if (state && buttons.data != oldButtonData)
        state->input->SetButtons(buttons);
}


void EmulatorMgr::ButtonReleased(Buttons::Button button)
{
    uint8_t oldButtonData = buttons.data;

    // Clear bit for button.
    buttons.data &= ~button;

    if (state && buttons.data != oldButtonData)
        state->input->SetButtons(buttons);
}


void EmulatorMgr::ThreadFunc(std::vector<uint8_t> *gameRomMemory)
{
    state = new State(drawFrameCallback);
    /*if (runbootRom)
        state->SetRomMemory(bootRomMemory, gameRomMemory);
    else*/
        state->SetRomMemory(*gameRomMemory);
    Emulator emulator(state);

    while (!quit)
    {
        if (!paused)
        {
            emulator.ProcessOpCode();
            state->PrintState();
            //state.timer->PrintTimerData();
            DBG("\n");
        }
    }

    delete gameRomMemory;
    delete state;
    state = NULL;
}