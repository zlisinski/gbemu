#pragma once

#include "../gbemu.h"
#include "../AbsFrameHandler.h"
#include "MainWindow.h"

class QtFrameHandler : public AbsFrameHandler
{
public:
    QtFrameHandler(MainWindow *mainWindow) :
        mainWindow(mainWindow)
    {}

    virtual ~QtFrameHandler() {}

    virtual void DrawFrame(uint32_t *frameBuffer)
    {
        mainWindow->FrameReady(frameBuffer);
    }

private:
    MainWindow *mainWindow;
};