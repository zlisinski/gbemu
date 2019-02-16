#include <QtWidgets/QGraphicsPixmapItem>
#include "DebugWindow.h"
#include "ui_DebugWindow.h"

#include "../Memory.h"

const uint8_t palette[4][3] = {{0xFF, 0xFF, 0xFF}, {0xB0, 0xB0, 0xB0}, {0x68, 0x68, 0x68}, {0x00, 0x00, 0x00}};
const int SCALE = 3;

DebugWindow::DebugWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugWindow),
    memory(NULL)
{
    ui->setupUi(this);

    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->gvTiles->setScene(scene);

    DrawFrame();
}


DebugWindow::~DebugWindow()
{
    delete ui;
}


void DebugWindow::SetMemory(uint8_t *memory)
{
    this->memory = memory;
}


void DebugWindow::DrawFrame()
{
    UpdateTileView();
}


void DebugWindow::UpdateTileView()
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->gvTiles->setScene(scene);
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

    ui->gvTiles->update();
}