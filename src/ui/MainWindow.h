#pragma once

#include <QtCore/QElapsedTimer>
#include <QtGamepad/QtGamepad>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>

class EmulatorMgr;
class QtFrameHandler;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Callback for Emulator to signal a frame is ready to be drawn.
    void FrameReady(uint32_t *frameBuffer);

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

private slots:
    void slotOpenRom();
    void slotTogglePause(bool checked);
    void slotToggleCapFps(bool checked);
    void slotQuit();
    void slotDrawFrame(uint32_t *frameBuffer);

signals:
    void SignalFrameReady(uint32_t *frameBuffer);
};
