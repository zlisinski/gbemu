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
class MemoryModel;


class DebuggerWindow : public QMainWindow, public DebuggerInterface
{
    Q_OBJECT

public:
    explicit DebuggerWindow(QWidget *parent = 0);
    ~DebuggerWindow();

    virtual void SetEmulatorObjects(Memory *newMemory, Cpu *newCpu, Interrupt *newInterrupt);

    virtual bool GetDebuggingEnabled() {return debuggingEnabled;}
    virtual bool ShouldRun(uint16_t pc);
    virtual void SetCurrentOp(uint16_t pc);

    virtual void MemoryChanged(uint16_t address, uint16_t len);

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    void UpdateStack();
    void UpdateWidgets(uint16_t pc);

    Ui::DebuggerWindow *ui;

    Cpu *cpu;
    Interrupt *interrupt;
    Memory *memory;
    uint16_t currentSp;

    std::atomic<bool> debuggingEnabled;
    std::atomic<bool> singleStep;
    std::atomic<uint16_t> runToAddress;

    DisassemblyModel *disassemblyModel;
    MemoryModel *memoryModel;

private slots:
    void SlotProcessUpdate(uint16_t pc);
    void SlotToggleDebugging(bool checked);
    void SlotStep();
    void SlotRunToLine();
    void SlotDisassembleAddress();
    void SlotReenableActions();

signals:
    void SignalDebuggerWindowClosed();
    void SignalUpdateReady(uint16_t pc);
    void SignalReenableActions();
};
