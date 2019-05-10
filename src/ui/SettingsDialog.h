#pragma once

#include <QtWidgets/QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    virtual void accept();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    void SaveSettings();

    Ui::SettingsDialog *ui;

    bool dirty;

private slots:
    void SlotToggleEnableBootRom(bool checked);
    void SlotClickBrowseBootRom();
    void SlotToggleEnableAudio(bool checked);
    void SlotAudioVolumeChanged(int value);
};