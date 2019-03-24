#pragma once

#include <QtCore/QElapsedTimer>
#include <QtGamepad/QtGamepad>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>

#include "../Display.h"

class DebuggerWindow;
class InfoWindow;
class EmulatorMgr;
class LogWindow;
class QtFrameHandler;

const int MAX_RECENT_FILES = 10;

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
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private:
    void SetupMenuBar();
    void SetupStatusBar();
    void SetupGamepad();
    void UpdateRecentFile(const QString &filename);
    void UpdateRecentFilesActions();
    void SetDisplayScale(int scale);
    void OpenRom(const QString &filename);

    QGraphicsView *graphicsView;
    QLabel *labelFps;
    QLabel *labelPause;

    bool paused;

    QtFrameHandler *frameHandler;
    EmulatorMgr *emulator;

    // FPS variables.
    QElapsedTimer fpsTimer;
    int frameCount;

    // Frame cap variables.
    QElapsedTimer frameCapTimer;
    int frameCapSetting;

    QGamepad *gamepad;
    QGamepadKeyNavigation *gamepadKeyNavigation;

    uint32_t frameBuffer[SCREEN_X * SCREEN_Y];
    int displayScale;

    InfoWindow *infoWindow;
    QAction *displayInfoWindowAction;
    LogWindow *logWindow;
    QAction *displayLogWindowAction;
    DebuggerWindow *debuggerWindow;
    QAction *displayDebuggerWindowAction;

    QAction *emuSaveStateAction;
    QAction *emuLoadStateAction;

    QAction *recentFilesActions[MAX_RECENT_FILES];

private slots:
    void SlotOpenRom();
    void SlotOpenRecentRom();
    void SlotReset();
    void SlotTogglePause(bool checked);
    void SlotEndEmulation();
    void SlotSetFpsCap();
    void SlotQuit();
    void SlotDrawFrame();
    void SlotShowMessageBox(const QString &message);
    void SlotSetDisplayScale();
    void SlotSetDisplayInfoWindow(bool checked);
    void SlotInfoWindowClosed();
    void SlotSetDisplayLogWindow(bool checked);
    void SlotLogWindowClosed();
    void SlotSetDisplayDebuggerWindow(bool checked);
    void SlotDebuggerWindowClosed();
    void SlotSaveState();
    void SlotLoadState();

signals:
    void SignalFrameReady();
    void SignalShowMessageBox(const QString &message);
};
