#include "State.h"


State::State(void (*drawFrameCallback)(uint32_t *)) :
    halted(false)
{
    memory = new Memory();
    interrupts = new Interrupt(memory->GetBytePtr(eRegIE), memory->GetBytePtr(eRegIF));
    timer = new Timer(memory->GetBytePtr(eRegTIMA), memory->GetBytePtr(eRegTMA),
                      memory->GetBytePtr(eRegTAC), memory->GetBytePtr(eRegDIV), interrupts);
    display = new Display(memory, interrupts, drawFrameCallback);
    input = new Input(memory->GetBytePtr(eRegP1), interrupts);
    serial = new Serial(memory->GetBytePtr(eRegSB), memory->GetBytePtr(eRegSC), interrupts);

    // Setup Memory observers.
    interrupts->AttachToMemorySubject(memory);
    timer->AttachToMemorySubject(memory);
    input->AttachToMemorySubject(memory);
    serial->AttachToMemorySubject(memory);

    // Setup Timer observers.
    display->AttachToTimerSubject(timer);
    memory->AttachToTimerSubject(timer);
    serial->AttachToTimerSubject(timer);
}


State::~State()
{
    delete serial;
    delete input;
    delete display;
    delete timer;
    delete interrupts;
    delete memory;
}


void State::SetRomMemory(std::array<uint8_t, BOOT_ROM_SIZE> &bootRomMemory, std::vector<uint8_t> &gameRomMemory)
{
    memory->SetRomMemory(bootRomMemory, gameRomMemory);
}


void State::SetRomMemory(std::vector<uint8_t> &gameRomMemory)
{
    memory->SetRomMemory(gameRomMemory);
}