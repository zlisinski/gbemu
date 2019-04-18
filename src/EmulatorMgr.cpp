#include <unistd.h>

#include "gbemu.h"
#include "AbsFrameHandler.h"
#include "Cpu.h"
#include "DebuggerInterface.h"
#include "Display.h"
#include "EmulatorMgr.h"
#include "InfoInterface.h"
#include "Input.h"
#include "Memory.h"
#include "Serial.h"
#include "Timer.h"


EmulatorMgr::EmulatorMgr(AbsFrameHandler *frameHandler, InfoInterface *infoInterface, DebuggerInterface *debuggerInterface) :
    paused(false),
    quit(false),
    frameHandler(frameHandler),
    infoInterface(infoInterface),
    debuggerInterface(debuggerInterface),
    buttons(),
    cpu(NULL),
    display(NULL),
    input(NULL),
    interrupts(NULL),
    memory(NULL),
    serial(NULL),
    timer(NULL)
{

}


EmulatorMgr::~EmulatorMgr()
{
    EndEmulation();
}


bool EmulatorMgr::LoadRom(const char *filename)
{
    EndEmulation();

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening %s\n", filename);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    gameRomMemory.clear();
    gameRomMemory.resize(size);

    size_t cnt = fread(&gameRomMemory[0], 1, size, file);
    if (cnt != (size_t)size)
    {
        printf("Only read %lu bytes of %ld from %s", cnt, size, filename);
        return false;
    }

    romFilename = filename;
    ramFilename = romFilename + ".ram";

    quit = false;

    memory = new Memory(infoInterface, debuggerInterface);
    interrupts = new Interrupt(memory);
    timer = new Timer(memory, interrupts);
    display = new Display(memory, interrupts, frameHandler);
    input = new Input(memory, interrupts);
    serial = new Serial(memory, interrupts);
    cpu = new Cpu(interrupts, memory, timer);

    /*if (runbootRom)
        memory->SetRomMemory(bootRomMemory, *gameRomMemory);
    else*/
    {
        memory->SetRomMemory(gameRomMemory);
        memory->LoadRam(ramFilename);
        SetBootState(memory, cpu);
    }
    workThread = std::thread(&EmulatorMgr::ThreadFunc, this);

    return true;
}


void EmulatorMgr::ResetEmulation()
{
    EndEmulation();
    LoadRom(romFilename.c_str());
}


void EmulatorMgr::PauseEmulation(bool pause)
{
    paused = pause;
}


void EmulatorMgr::EndEmulation()
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

    if (input && buttons.data != oldButtonData)
        input->SetButtons(buttons);
}


void EmulatorMgr::ButtonReleased(Buttons::Button button)
{
    uint8_t oldButtonData = buttons.data;

    // Clear bit for button.
    buttons.data &= ~button;

    if (input && buttons.data != oldButtonData)
        input->SetButtons(buttons);
}


void EmulatorMgr::SaveState(int slot)
{
    // Lock mutex to make the worker thread wait while the state is saved.
    std::lock_guard<std::mutex> lock(saveStateMutex);

    // Open a temp file so that errors writing don't mess up an existing save file.
    char tempFilename[] = "tmpsav.XXXXXX";
    int fd = mkstemp(tempFilename);
    FILE *file = fdopen(fd, "w");
    if (file == NULL)
    {
        LogError("Error opening save state file %s: %s", tempFilename, strerror(errno));
        frameHandler->MessageBox("Error opening save state file");
        return;
    }

    bool success = true;

    // Write header.
    if (!fwrite("ZLGB", 4, 1, file))
        success = false;

    // Write version.
    const uint16_t version = 2;
    if (!fwrite(&version, sizeof(version), 1, file))
        success = false;

    // Write data.
    success &= memory->SaveState(file);
    success &= interrupts->SaveState(file);
    success &= timer->SaveState(file);
    success &= display->SaveState(file);
    success &= input->SaveState(file);
    success &= serial->SaveState(file);
    success &= cpu->SaveState(file);

    fclose(file);

    if (success == false)
    {
        LogError("Error saving state");
        frameHandler->MessageBox("Error saving state");
        //unlink(tempFilename);
        return;
    }

    std::string saveFilename = romFilename + ".sav" + std::to_string(slot);
    if (rename(tempFilename, saveFilename.c_str()))
    {
        LogError("Error renaming temp save state file %s to %s: %s", tempFilename, saveFilename.c_str(), strerror(errno));
        frameHandler->MessageBox("Error renaming temp save state file");
        return;
    }

    LogError("Saved state to %s", saveFilename.c_str());
}


void EmulatorMgr::LoadState(int slot)
{
    std::string loadFilename = romFilename + ".sav" + std::to_string(slot);
    FILE *file = fopen(loadFilename.c_str(), "rb");
    if (file == NULL)
    {
        LogError("Error opening save state file %s: %s", loadFilename.c_str(), strerror(errno));
        frameHandler->MessageBox("Error opening save state file");
        return;
    }

    // Read header.
    const int headerLen = 4;
    char header[headerLen + 1] = {0};
    size_t cnt = fread(header, 1, headerLen, file);
    if (cnt != headerLen)
    {
        LogError("Error reading header from save state file. Only read %u bytes.", cnt);
        frameHandler->MessageBox("Error reading header from save state file.");
        fclose(file);
        return;
    }
    if (strcmp(header, "ZLGB"))
    {
        LogError("Save state header doesn't match expected value: %s", header);
        frameHandler->MessageBox("Save state header doesn't match expected value.");
        fclose(file);
        return;
    }

    // Get version.
    uint16_t version = 0;
    if (!fread(&version, 2, 1, file))
    {
        LogError("Error reading version from state file.");
        frameHandler->MessageBox("Error reading version from state file.");
        fclose(file);
        return;
    }
    if (version == 0x3130)
    {
        // The first version of the save state format saved the version as ASCII "01".
        version = 1;
    }

    // Create new objects so if there is an error loading, the current game doesn't get killed.
    Memory *newMemory = new Memory(infoInterface, debuggerInterface);
    Interrupt *newInterrupts = new Interrupt(newMemory);
    Timer *newTimer = new Timer(memory, newInterrupts);
    Display *newDisplay = new Display(newMemory, newInterrupts, frameHandler);
    Input *newInput = new Input(newMemory, newInterrupts);
    Serial *newSerial = new Serial(newMemory, newInterrupts);
    Cpu *newCpu = new Cpu(newInterrupts, newMemory, newTimer);

    newMemory->SetRomMemory(gameRomMemory);

    bool success = true;

    // Load data.
    success &= newMemory->LoadState(version, file);
    success &= newInterrupts->LoadState(version, file);
    success &= newTimer->LoadState(version, file);
    success &= newDisplay->LoadState(version, file);
    success &= newInput->LoadState(version, file);
    success &= newSerial->LoadState(version, file);
    success &= newCpu->LoadState(version, file);

    fclose(file);

    if (success == false)
    {
        LogError("Error loading state");
        frameHandler->MessageBox("Error loading state");
        return;
    }

    // End the current game.
    EndEmulation();

    // Replace pointers with new objects.
    memory = newMemory;
    interrupts = newInterrupts;
    timer = newTimer;
    display = newDisplay;
    input = newInput;
    serial = newSerial;
    cpu = newCpu;

    // Start emulation.
    paused = false;
    quit = false;
    workThread = std::thread(&EmulatorMgr::ThreadFunc, this);

    LogError("Loaded save file %s", loadFilename.c_str());
}


void EmulatorMgr::ThreadFunc()
{
    try
    {
        // Setup Timer observers.
        display->AttachToTimerSubject(timer);
        memory->AttachToTimerSubject(timer);
        serial->AttachToTimerSubject(timer);

        if (infoInterface)
            infoInterface->SetMemory(memory->GetBytePtr(0));

        if (debuggerInterface)
            debuggerInterface->SetEmulatorObjects(memory, cpu, interrupts);

        while (!quit)
        {
            // Block this thread while state is being saved.
            std::lock_guard<std::mutex> lock(saveStateMutex);

            // Run multiple instruction per mutex lock to reduce the impact of locking the mutex.
            for (int i = 0; i < 100; i++)
            {
                if (!paused && debuggerInterface->ShouldRun(cpu->reg.pc))
                {
                    cpu->ProcessOpCode();
                    if (debuggerInterface->GetDebuggingEnabled())
                        debuggerInterface->SetCurrentOp(cpu->reg.pc);
                    cpu->PrintState();
                    //timer->PrintTimerData();
                }
                else
                {
                    // Sleep to avoid pegging the CPU when paused.
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
        }

        memory->SaveRam(ramFilename);
    }
    catch(const std::exception& e)
    {
        frameHandler->MessageBox(e.what());
    }

    if (infoInterface)
        infoInterface->SetMemory(NULL);
    if (debuggerInterface)
        debuggerInterface->SetEmulatorObjects(NULL, NULL, NULL);

    delete cpu;
    cpu = NULL;
    delete display;
    display = NULL;
    delete input;
    input = NULL;
    delete interrupts;
    interrupts = NULL;
    delete serial;
    serial = NULL;
    delete timer;
    timer = NULL;
    delete memory;
    memory = NULL;
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