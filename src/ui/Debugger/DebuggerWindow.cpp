#include <QtCore/QSettings>

#include "../../Cpu.h"
#include "../../Interrupt.h"
#include "../../Memory.h"
#include "../UiUtils.h"
#include "DebuggerWindow.h"
#include "DisassemblyModel.h"
#include "ui_DebuggerWindow.h"


DebuggerWindow::DebuggerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebuggerWindow),
    cpu(NULL),
    interrupt(NULL),
    debuggingEnabled(false),
    step(true),
    disassemblyModel(new DisassemblyModel(palette(), this))
{
    ui->setupUi(this);

    //ui->disassemblyView->setFocusPolicy(Qt::NoFocus);

    qRegisterMetaType<QItemSelection>();
    qRegisterMetaType<uint16_t>("uint16_t");

    QSettings settings;
    restoreGeometry(settings.value("DebuggerWindowGeometry").toByteArray());
    restoreState(settings.value("DebuggerWindowState").toByteArray());
    ui->splitter->restoreState(settings.value("DebuggerWindowSplitterState").toByteArray());

    ui->disassemblyView->setModel(disassemblyModel);

    connect(ui->actionToggleDebugging, SIGNAL(triggered(bool)), this, SLOT(SlotToggleDebugging(bool)));
    connect(ui->actionStep, SIGNAL(triggered()), this, SLOT(SlotStep()));
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
    settings.setValue("DebuggerWindowSplitterState", ui->splitter->saveState());

    emit SignalDebuggerWindowClosed();

    QWidget::closeEvent(event);
}


void DebuggerWindow::SetCurrentOp(uint16_t pc)
{
    //This function runs in the thread context of the Emulator worker thread.
    step = false;

    emit SignalUpdateReady(pc);
}


void DebuggerWindow::SlotProcessUpdate(uint16_t pc)
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


void DebuggerWindow::SlotToggleDebugging(bool checked)
{
    debuggingEnabled = checked;

    if (debuggingEnabled)
        ui->actionToggleDebugging->setText("Stop Debugging");
    else
        ui->actionToggleDebugging->setText("Start Debugging");
}


void DebuggerWindow::SlotStep()
{
    step = true;
}