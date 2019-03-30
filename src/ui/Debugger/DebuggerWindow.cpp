#include <QtCore/QSettings>

#include "../../Cpu.h"
#include "../../Interrupt.h"
#include "../../Memory.h"
#include "../UiUtils.h"
#include "AddressDialog.h"
#include "DebuggerWindow.h"
#include "DisassemblyModel.h"
#include "MemoryModel.h"
#include "ui_DebuggerWindow.h"


DebuggerWindow::DebuggerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebuggerWindow),
    cpu(NULL),
    interrupt(NULL),
    memory(NULL),
    currentSp(0),
    debuggingEnabled(false),
    singleStep(true),
    runToAddress(0xFFFF),
    disassemblyModel(new DisassemblyModel(palette(), this)),
    memoryModel(new MemoryModel(this))
{
    ui->setupUi(this);
    ui->actionToggleDebugging->setChecked(debuggingEnabled);
    SlotToggleDebugging(debuggingEnabled);
    ui->menuView->addAction(ui->dockCallStack->toggleViewAction());
    ui->menuView->addAction(ui->dockMemory->toggleViewAction());
    ui->menuView->addAction(ui->dockRegisters->toggleViewAction());
    ui->menuView->addAction(ui->dockStack->toggleViewAction());

    qRegisterMetaType<QItemSelection>();
    qRegisterMetaType<uint16_t>("uint16_t");

    QSettings settings;
    restoreGeometry(settings.value("DebuggerWindowGeometry").toByteArray());
    restoreState(settings.value("DebuggerWindowState").toByteArray());

    ui->disassemblyView->setModel(disassemblyModel);
    ui->memoryView->setModel(memoryModel);

    ui->memoryView->resizeColumnsToContents();

    connect(ui->actionToggleDebugging, SIGNAL(triggered(bool)), this, SLOT(SlotToggleDebugging(bool)));
    connect(ui->actionStep, SIGNAL(triggered()), this, SLOT(SlotStep()));
    connect(ui->actionRunToLine, SIGNAL(triggered()), this, SLOT(SlotRunToLine()));
    connect(ui->actionDisassemble, SIGNAL(triggered()), this, SLOT(SlotDisassembleAddress()));
    connect(this, SIGNAL(SignalDebuggerWindowClosed()), parent, SLOT(SlotDebuggerWindowClosed()));
    connect(this, SIGNAL(SignalUpdateReady(uint16_t)), this, SLOT(SlotProcessUpdate(uint16_t)));
}


DebuggerWindow::~DebuggerWindow()
{
    delete ui;
}


void DebuggerWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("DebuggerWindowGeometry", saveGeometry());
    settings.setValue("DebuggerWindowState", saveState());

    emit SignalDebuggerWindowClosed();

    QWidget::closeEvent(event);
}


void DebuggerWindow::SetMemory(Memory *newMemory)
{
    memory = newMemory;

    memoryModel->SetMemory(memory);
}


bool DebuggerWindow::ShouldRun()
{
    // Run if we're in single step mode, or we have a run-to address and we're not there.
    return debuggingEnabled == true && (singleStep == true || (runToAddress != 0xFFFF && currentAddress != runToAddress));
}


void DebuggerWindow::SetCurrentOp(uint16_t pc)
{
    //This function runs in the thread context of the Emulator worker thread.

    currentAddress = pc;

    // Stop single step mode.
    singleStep = false;

    // If we've hit the run-to address, reset the variable.
    // 0xFFFF is a data register, so it's safe to use for an invalid address value.
    if (runToAddress == currentAddress)
        runToAddress = 0xFFFF;

    emit SignalUpdateReady(pc);
}


void DebuggerWindow::MemoryChanged(uint16_t address, uint16_t len)
{
    // Memory has changed, so the disassembled opcodes are no longer valid.
    disassemblyModel->RemoveRows(address, len);

    // Only update memory table when debugging to avoid slowing things down.
    if (debuggingEnabled == true)
    {
        memoryModel->InvalidateMemory(address, len);
    }
}


void DebuggerWindow::UpdateStack()
{
    if (cpu == NULL || memory == NULL)
        return;

    currentSp = cpu->reg.sp;

    // Let the view determine how many rows to be displayed.
    const int rowCount = ui->stackView->rowCount();

    ui->stackView->clearContents();

    for (int i = 0; i < rowCount; i++)
    {
        uint16_t address = currentSp + (i * 2);
        QTableWidgetItem *item = new QTableWidgetItem(UiUtils::FormatHexWord(address));
        ui->stackView->setItem(i, 0, item);

        uint16_t value = memory->ReadByte(address) | (memory->ReadByte(address + 1) << 8);
        item = new QTableWidgetItem(UiUtils::FormatHexWord(value));
        ui->stackView->setItem(i, 1, item);
    }
}


void DebuggerWindow::UpdateWidgets(uint16_t pc)
{
    disassemblyModel->AddRow(pc, memory->GetBytePtr(0));

    int rowIndex = disassemblyModel->GetRowIndex(pc);
    if (rowIndex >= 0)
    {
        disassemblyModel->SetCurrentRow(rowIndex);
        ui->disassemblyView->scrollTo(ui->disassemblyView->model()->index(rowIndex, 0));
        ui->disassemblyView->viewport()->update();
    }

    if (cpu != NULL)
    {
        ui->chkFlagZ->setChecked(cpu->reg.flags.z);
        ui->chkFlagN->setChecked(cpu->reg.flags.n);
        ui->chkFlagH->setChecked(cpu->reg.flags.h);
        ui->chkFlagC->setChecked(cpu->reg.flags.c);
        ui->txtAF->setText(UiUtils::FormatHexWord(cpu->reg.af));
        ui->txtBC->setText(UiUtils::FormatHexWord(cpu->reg.bc));
        ui->txtDE->setText(UiUtils::FormatHexWord(cpu->reg.de));
        ui->txtHL->setText(UiUtils::FormatHexWord(cpu->reg.hl));
        ui->txtPC->setText(UiUtils::FormatHexWord(cpu->reg.pc));
        ui->txtSP->setText(UiUtils::FormatHexWord(cpu->reg.sp));

        if (currentSp != cpu->reg.sp)
            UpdateStack();
    }

    if (memory != NULL)
    {
        ui->txtRegP1->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegP1)));
        ui->txtRegIE->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegIE)));
        ui->txtRegIF->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegIF)));
        ui->txtRegDIV->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegDIV)));
        ui->txtRegTIMA->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegTIMA)));
        ui->txtRegTMA->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegTMA)));
        ui->txtRegTAC->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegTAC)));
        ui->txtRegLCDC->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegLCDC)));
        ui->txtRegSTAT->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegSTAT)));
        ui->txtRegSCX->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegSCX)));
        ui->txtRegSCY->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegSCY)));
        ui->txtRegWX->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegWX)));
        ui->txtRegWY->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegWY)));
        ui->txtRegLY->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegLY)));
        ui->txtRegLYC->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegLYC)));
        ui->txtRegDMA->setText(UiUtils::FormatHexWord(memory->ReadByte(eRegDMA)));
        ui->txtRegCurDMA->setText(UiUtils::FormatHexWord(OAM_RAM_START + memory->GetDmaOffset()));
    }

    if (interrupt != NULL)
        ui->chkRegIME->setChecked(interrupt->Enabled());
}


void DebuggerWindow::SlotProcessUpdate(uint16_t pc)
{
    UpdateWidgets(pc);

    // Add current instruction to call stack.
    Opcode opcode = Opcode::GetOpcode(pc, memory->GetBytePtr(pc));
    new QListWidgetItem(opcode.ToString(), ui->callStackView);
}


void DebuggerWindow::SlotToggleDebugging(bool checked)
{
    debuggingEnabled = checked;

    if (debuggingEnabled)
    {
        ui->actionToggleDebugging->setText("Stop Debugging");
        ui->actionStep->setEnabled(true);
        ui->actionRunToLine->setEnabled(true);
        ui->actionDisassemble->setEnabled(true);

        // Update memory table with new values.
        memoryModel->SetMemory(memory);

        // Update widgets with new values.
        if (cpu != NULL)
            UpdateWidgets(cpu->reg.pc);

        // Clear the call stack. It would be too slow to log the call stack when not debugging.
        ui->callStackView->clear();
    }
    else
    {
        ui->actionToggleDebugging->setText("Start Debugging");
        ui->actionStep->setEnabled(false);
        ui->actionRunToLine->setEnabled(false);
        ui->actionDisassemble->setEnabled(false);
    }
}


void DebuggerWindow::SlotStep()
{
    singleStep = true;
}


void DebuggerWindow::SlotRunToLine()
{
    QItemSelectionModel *selection = ui->disassemblyView->selectionModel();

    if (!selection->hasSelection())
    {
        UiUtils::MessageBox("No row selected");
        return;
    }

    runToAddress = disassemblyModel->GetAddressOfRow(selection->selectedRows()[0].row());

    // Clear selection so we can see when the line has been reached. TODO: Add "wait" dialog while running.
    ui->disassemblyView->clearSelection();
}


void DebuggerWindow::SlotDisassembleAddress()
{
    if (memory == NULL)
    {
        UiUtils::MessageBox("Game not loaded");
        return;
    }

    AddressDialog dialog(this);
    dialog.setModal(true);
    int ret = dialog.exec();

    if (ret == 1)
    {
        disassemblyModel->AddRow(dialog.address, memory->GetBytePtr(0));
    }
}