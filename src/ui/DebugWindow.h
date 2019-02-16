#pragma once

#include <QtWidgets/QDialog>

#include "../DebugInterface.h"

namespace Ui {
class DebugWindow;
}

class DebugWindow : public QDialog, public DebugInterface
{
    Q_OBJECT

public:
    explicit DebugWindow(QWidget *parent = 0);
    ~DebugWindow();

    virtual void SetMemory(uint8_t *memory);

    void DrawFrame();

private:
    void UpdateTileView();

    Ui::DebugWindow *ui;

    uint8_t *memory;
};