#pragma once

#include <QtWidgets/QDialog>

#include "../InfoInterface.h"

namespace Ui {
class InfoWindow;
}

class QRadioButton;

class InfoWindow : public QDialog, public InfoInterface
{
    Q_OBJECT

public:
    explicit InfoWindow(QWidget *parent = 0);
    ~InfoWindow();

    virtual void SetMemory(uint8_t *memory) {this->memory = memory;}
    virtual void SetMbcType(MbcTypes mbcType) {this->mbcType = mbcType;}
    virtual void SetRomBanks(int count) {romBanks = count;}
    virtual void SetRamBanks(int count) {ramBanks = count;}
    virtual void SetMappedRomBank(int bank) {mappedRomBank = bank;}
    virtual void SetMappedRamBank(int bank) {mappedRamBank = bank;}
    virtual void SetBatteryBackedRam(bool hasBattery) {batteryBackedRam = hasBattery;}

    void DrawFrame();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    void UpdateTileView();
    void UpdateMemoryView();

    void SetRadioButton(bool val, QRadioButton *radio0, QRadioButton *radio1);

    Ui::InfoWindow *ui;

    uint8_t *memory;
    MbcTypes mbcType;
    int romBanks;
    int ramBanks;
    int mappedRomBank;
    int mappedRamBank;
    bool batteryBackedRam;

signals:
    void SignalInfoWindowClosed();
};