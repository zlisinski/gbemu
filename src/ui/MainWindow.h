#pragma once

#include <QtCore/QElapsedTimer>
#include <QtGamepad/QtGamepad>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>

#include "../Display.h"

class DebugWindow;
class EmulatorMgr;
class QtFrameHandler;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Callback for Emulator to signal a frame is ready to be drawn.
    void FrameReady(uint32_t *displayFrameBuffer);
    // Callback for Emulator to show message box.
    void RequestMessageBox(const std::string &message);

    // Don't allow copy and assignment.
    MainWindow(const MainWindow&) = delete;
    void operator=(const MainWindow&) = delete;

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    void SetupMenuBar();
    void SetupStatusBar();
    void SetupGamepad();

    QGraphicsView *graphicsView;
    QLabel *labelFps;
    QLabel *labelPause;

    bool paused;
    bool capFps;

    QtFrameHandler *frameHandler;
    EmulatorMgr *emulator;

    // FPS variables.
    QElapsedTimer fpsTimer;
    int frameCount;

    // Frame cap variables.
    QElapsedTimer frameCapTimer;

    QGamepad *gamepad;
    QGamepadKeyNavigation *gamepadKeyNavigation;

    uint32_t frameBuffer[SCREEN_X * SCREEN_Y];

    DebugWindow *debugWindow;

private slots:
    void slotOpenRom();
    void slotReset();
    void slotTogglePause(bool checked);
    void slotEndEmulation();
    void slotToggleCapFps(bool checked);
    void slotQuit();
    void slotDrawFrame();
    void slotShowMessageBox(const QString &message);

signals:
    void SignalFrameReady();
    void SignalShowMessageBox(const QString &message);
};
