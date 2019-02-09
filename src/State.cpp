#include "State.h"


State::State(void (*drawFrameCallback)(uint32_t *)) :
    f(0),
    a(0),
    bc(0),
    de(0),
    hl(0),
    pc(0),
    sp(0),
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
    SetBootState();
}


void State::SetBootState()
{
    // Set state to what it would be after running the BIOS.
    a = 0x01;
    f = 0xB0;
    bc = 0x0013;
    de = 0x00D8;
    hl = 0x014D;
    sp = 0xFFFE;
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

    pc = 0x0100;
}