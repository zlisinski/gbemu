#pragma once

#include <QtWidgets/QDialog>

#include "../DebugInterface.h"

namespace Ui {
class DebugWindow;
}

class QRadioButton;

class DebugWindow : public QDialog, public DebugInterface
{
    Q_OBJECT

public:
    explicit DebugWindow(QWidget *parent = 0);
    ~DebugWindow();

    virtual void SetMemory(uint8_t *memory) {this->memory = memory;}
    virtual void SetMbcType(MbcTypes mbcType) {this->mbcType = mbcType;}
    virtual void SetRomBanks(int count) {romBanks = count;}
    virtual void SetRamBanks(int count) {ramBanks = count;}
    virtual void SetMappedRomBank(int bank) {mappedRomBank = bank;}
    virtual void SetMappedRamBank(int bank) {mappedRamBank = bank;}

    void DrawFrame();

private:
    void UpdateTileView();
    void UpdateMemoryView();

    QString FormatByte(uint8_t num);
    QString FormatWord(uint16_t num);
    void SetRadioButton(bool val, QRadioButton *radio0, QRadioButton *radio1);

    Ui::DebugWindow *ui;

    uint8_t *memory;
    MbcTypes mbcType;
    int romBanks;
    int ramBanks;
    int mappedRomBank;
    int mappedRamBank;
};