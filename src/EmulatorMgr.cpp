#include "gbemu.h"
#include "Cpu.h"
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
    Cpu cpu(state);
    /*if (runbootRom)
        state->SetRomMemory(bootRomMemory, gameRomMemory);
    else*/
    {
        state->SetRomMemory(*gameRomMemory);
        SetBootState(state->memory, &cpu);
    }

    while (!quit)
    {
        if (!paused)
        {
            cpu.ProcessOpCode();
            //state->PrintState();
            //state.timer->PrintTimerData();
            DBG("\n");
        }
    }

    delete gameRomMemory;
    delete state;
    state = NULL;
}


void EmulatorMgr::SetBootState(Memory *memory, Cpu *cpu)
{
    // Set state to what it would be after running the BIOS.
    cpu->reg.a = 0x01;
    cpu->reg.f = 0xB0;
    cpu->reg.bc = 0x0013;
    cpu->reg.de = 0x00D8;
    cpu->reg.hl = 0x014D;
    cpu->reg.sp = 0xFFFE;
    cpu->reg.pc = 0x0100;
    memory->WriteByte(eRegTIMA, 0x00);
    memory->WriteByte(eRegTMA, 0x00);
    memory->WriteByte(eRegTAC, 0x00);
    memory->WriteByte(eRegNR10, 0x80);
    memory->WriteByte(eRegNR11, 0xBF);
    memory->WriteByte(eRegNR12, 0xF3);
    memory->WriteByte(eRegNR14, 0xBF);
    memory->WriteByte(eRegNR21, 0x3F);
    memory->WriteByte(eRegNR22, 0x00);
    memory->WriteByte(eRegNR24, 0xBF);
    memory->WriteByte(eRegNR30, 0x7F);
    memory->WriteByte(eRegNR31, 0xFF);
    memory->WriteByte(eRegNR32, 0x9F);
    memory->WriteByte(eRegNR34, 0xBF);
    memory->WriteByte(eRegNR41, 0xFF);
    memory->WriteByte(eRegNR42, 0x00);
    memory->WriteByte(eRegNR43, 0x00);
    memory->WriteByte(eRegNR44, 0xBF);
    memory->WriteByte(eRegNR50, 0x77);
    memory->WriteByte(eRegNR51, 0xF3);
    memory->WriteByte(eRegNR52, 0xF1);
    memory->WriteByte(eRegLCDC, 0x91);
    memory->WriteByte(eRegSCY, 0x00);
    memory->WriteByte(eRegSCX, 0x00);
    memory->WriteByte(eRegLYC, 0x00);
    memory->WriteByte(eRegBGP, 0xFC);
    memory->WriteByte(eRegOBP0, 0xFF);
    memory->WriteByte(eRegOBP1, 0xFF);
    memory->WriteByte(eRegWY, 0x00);
    memory->WriteByte(eRegWX, 0x00);
    memory->WriteByte(eRegIE, 0x00);
}