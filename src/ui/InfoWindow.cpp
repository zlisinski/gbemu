#include <memory>
#include <QtCore/QSettings>
#include <QtWidgets/QGraphicsPixmapItem>
#include "InfoWindow.h"
#include "SettingsConstants.h"
#include "ui_InfoWindow.h"
#include "UiUtils.h"

#include "../Memory.h"

const uint8_t palette[4][3] = {{0xFF, 0xFF, 0xFF}, {0xB0, 0xB0, 0xB0}, {0x68, 0x68, 0x68}, {0x00, 0x00, 0x00}};
const int SCALE = 3;
const QMap<MbcTypes, QString> MBC_NAMES {
    {eMbcNone, "None"},
    {eMbc1, "MBC1"},
    {eMbc2, "MBC2"},
    {eMbc3, "MBC3"},
    {eMbc5, "MBC5"},
};

InfoWindow::InfoWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoWindow),
    memory(NULL),
    mbcType(eMbcNone),
    romBanks(0),
    ramBanks(0),
    mappedRomBank(0),
    mappedRamBank(0),
    batteryBackedRam(false)
{
    ui->setupUi(this);

    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->gvTiles->setScene(scene);

    QSettings settings;
    restoreGeometry(settings.value(SETTINGS_INFOWINDOW_GEOMETRY).toByteArray());

    connect(this, SIGNAL(SignalInfoWindowClosed()), parent, SLOT(SlotInfoWindowClosed()));

    DrawFrame();
}


InfoWindow::~InfoWindow()
{
    delete ui;
}


void InfoWindow::DrawFrame()
{
    UpdateTileView();
    UpdateMemoryView();
}


void InfoWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue(SETTINGS_INFOWINDOW_GEOMETRY, saveGeometry());

    emit SignalInfoWindowClosed();

    QWidget::closeEvent(event);
}


void InfoWindow::UpdateTileView()
{
    ui->gvTiles->scene()->clear();
    ui->gvTiles->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern));

    // Draw grid.
    for (int i = 0; i < 25; i++)
    {
        int len = i * ((SCALE * 8) + 1);
        ui->gvTiles->scene()->addLine(0, len, 400, len);
    }
    for (int i = 0; i < 17; i++)
    {
        int len = i * ((SCALE * 8) + 1);
        ui->gvTiles->scene()->addLine(len, 0, len, 600);
    }

    if (memory != NULL)
    {
        const uint8_t * const tilesetData = &memory[0x8000];
        const uint8_t regBGP = memory[eRegBGP];

        for (int tile = 0; tile < 384; tile++)
        {
            QImage img(8, 8, QImage::Format_RGB32);

            for (int y = 0; y < 8; y++)
            {
                const uint8_t * const tileData = &tilesetData[(tile * 16) + (y * 2)];

                for (int x = 0; x < 8; x++)
                {
                    const uint8_t lowBit = ((tileData[0] >> (7 - x)) & 0x01);
                    const uint8_t highBit = ((tileData[1] >> (7 - x)) & 0x01);
                    const uint8_t pixelVal = lowBit | (highBit << 1);
                    const uint8_t * const color = ::palette[(regBGP >> (pixelVal * 2)) & 0x03];

                    img.setPixel(x, y, (color[0] << 16) | (color[1] << 8) | color[2]);
                }
            }

            QGraphicsPixmapItem *pixmap = ui->gvTiles->scene()->addPixmap(QPixmap::fromImage(img));
            pixmap->setScale(SCALE);
            int xpos = 1 + ((tile % 16) * 8 * SCALE) + (tile % 16);
            int ypos = 1 + ((tile / 16) * 8 * SCALE) + (tile / 16);
            pixmap->setPos(xpos, ypos);
        }
    }
}


void InfoWindow::UpdateMemoryView()
{
    if (memory == NULL)
    {
        ui->txtLCDC->setText("");
        SetRadioButton(0, ui->rbBGEnable0, ui->rbBGEnable1);
        SetRadioButton(0, ui->rbSpriteEnable0, ui->rbSpriteEnable1);
        SetRadioButton(0, ui->rbSpriteSize0, ui->rbSpriteSize1);
        SetRadioButton(0, ui->rbBGTilemap0, ui->rbBGTilemap1);
        SetRadioButton(0, ui->rbWindowTileset0, ui->rbWindowTileset1);
        SetRadioButton(0, ui->rbWindowEnable0, ui->rbWindowEnable1);
        SetRadioButton(0, ui->rbWindowTilemap0, ui->rbWindowTilemap1);
        SetRadioButton(0, ui->rbPower0, ui->rbPower1);

        ui->txtSCX->setText("");
        ui->txtSCY->setText("");
        ui->txtWX->setText("");
        ui->txtWY->setText("");

        ui->txtMbcType->setText(MBC_NAMES[eMbcNone]);
        ui->txtRomBanks->setText("");
        ui->txtRamBanks->setText("");
        ui->txtMappedRom->setText("");
        ui->txtMappedRam->setText("");
        ui->chkBatteryBackedRam->setChecked(false);
    }
    else
    {
        uint8_t lcdc = memory[eRegLCDC];
        ui->txtLCDC->setText(UiUtils::FormatHexByte(lcdc));
        SetRadioButton(lcdc & 0x01, ui->rbBGEnable0, ui->rbBGEnable1);
        SetRadioButton(lcdc & 0x02, ui->rbSpriteEnable0, ui->rbSpriteEnable1);
        SetRadioButton(lcdc & 0x04, ui->rbSpriteSize0, ui->rbSpriteSize1);
        SetRadioButton(lcdc & 0x08, ui->rbBGTilemap0, ui->rbBGTilemap1);
        SetRadioButton(lcdc & 0x10, ui->rbWindowTileset0, ui->rbWindowTileset1);
        SetRadioButton(lcdc & 0x20, ui->rbWindowEnable0, ui->rbWindowEnable1);
        SetRadioButton(lcdc & 0x40, ui->rbWindowTilemap0, ui->rbWindowTilemap1);
        SetRadioButton(lcdc & 0x80, ui->rbPower0, ui->rbPower1);

        ui->txtSCX->setText(UiUtils::FormatHexByte(memory[eRegSCX]));
        ui->txtSCY->setText(UiUtils::FormatHexByte(memory[eRegSCY]));
        ui->txtWX->setText(UiUtils::FormatHexByte(memory[eRegWX]));
        ui->txtWY->setText(UiUtils::FormatHexByte(memory[eRegWY]));

        ui->txtMbcType->setText(MBC_NAMES[mbcType]);
        ui->txtRomBanks->setText(QString::number(romBanks));
        ui->txtRamBanks->setText(QString::number(ramBanks));
        ui->txtMappedRom->setText(QString::number(mappedRomBank));
        ui->txtMappedRam->setText(QString::number(mappedRamBank));
        ui->chkBatteryBackedRam->setChecked(batteryBackedRam);
    }
}


void InfoWindow::SetRadioButton(bool val, QRadioButton *radio0, QRadioButton *radio1)
{
    if (val)
        radio1->setChecked(true);
    else
        radio0->setChecked(true);
}