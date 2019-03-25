#pragma once

#include <QtWidgets/QMainWindow>

#include "../../gbemu.h"
#include "../../DebuggerInterface.h"

namespace Ui {
class DebuggerWindow;
}

class Cpu;
class DisassemblyModel;
class Interrupt;
class Memory;


class DebuggerWindow : public QMainWindow, public DebuggerInterface
{
    Q_OBJECT

public:
    explicit DebuggerWindow(QWidget *parent = 0);
    ~DebuggerWindow();

    virtual void SetMemory(Memory *memory) {this->memory = memory;}
    virtual void SetCpu(Cpu *cpu) {this->cpu = cpu;}
    virtual void SetInterrupt(Interrupt *interrupt) {this->interrupt = interrupt;}

    virtual bool GetDebuggingEnabled() {return debuggingEnabled;}
    virtual bool ShouldRun();
    virtual void SetCurrentOp(uint16_t pc);

    virtual void MemoryChanged(uint16_t address, uint16_t len);

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    Ui::DebuggerWindow *ui;

    Cpu *cpu;
    Interrupt *interrupt;
    Memory *memory;

    bool debuggingEnabled;
    bool singleStep;
    uint16_t runToAddress;
    uint16_t currentAddress;

    DisassemblyModel *disassemblyModel;

private slots:
    void SlotProcessUpdate(uint16_t pc);
    void SlotToggleDebugging(bool checked);
    void SlotStep();
    void SlotRunToLine();

signals:
    void SignalDebuggerWindowClosed();
    void SignalUpdateReady(uint16_t pc);
};
