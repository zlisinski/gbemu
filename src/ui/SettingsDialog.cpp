#include <QtCore/QSettings>
#include <QtWidgets/QtWidgets>

#include "SettingsConstants.h"
#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    dirty(false)
{
    ui->setupUi(this);

    QSettings settings;
    ui->chkEnableBootRom->setChecked(settings.value(SETTINGS_BOOTROM_ENABLED, false).toBool());
    ui->txtBootRomFilename->setText(settings.value(SETTINGS_BOOTROM_PATH).toString());

    ui->btnBrowseBootRom->setEnabled(ui->chkEnableBootRom->isChecked());
    ui->txtBootRomFilename->setEnabled(ui->chkEnableBootRom->isChecked());

    connect(ui->chkEnableBootRom, SIGNAL(toggled(bool)), this, SLOT(SlotToggleEnableBootRom(bool)));
    connect(ui->btnBrowseBootRom, SIGNAL(clicked()), this, SLOT(SlotClickBrowseBootRom()));
}


SettingsDialog::~SettingsDialog()
{
    delete ui;
}


void SettingsDialog::accept()
{
    SaveSettings();

    QDialog::accept();
}


void SettingsDialog::closeEvent(QCloseEvent *event)
{
    if (dirty)
    {
        QMessageBox msg(QMessageBox::Warning, "Unsaved changes",
                        "There are unsaved changes. Do you want to save them?",
                        QMessageBox::Save | QMessageBox::Discard, this);

        if (msg.exec() == QMessageBox::Save)
            SaveSettings();
    }

    QWidget::closeEvent(event);
}


void SettingsDialog::SaveSettings()
{
    QSettings settings;
    settings.setValue(SETTINGS_BOOTROM_ENABLED, ui->chkEnableBootRom->isChecked());
    settings.setValue(SETTINGS_BOOTROM_PATH, ui->txtBootRomFilename->text());
}


void SettingsDialog::SlotToggleEnableBootRom(bool checked)
{
    ui->btnBrowseBootRom->setEnabled(checked);
    ui->txtBootRomFilename->setEnabled(checked);

    dirty = true;
}


void SettingsDialog::SlotClickBrowseBootRom()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select Boot ROM File", "", "*.gb");

    if (filename != "")
    {
        ui->txtBootRomFilename->setText(filename);
        dirty = true;
    }
}