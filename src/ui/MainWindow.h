#pragma once

#include <QtCore/QElapsedTimer>
#include <QtGamepad/QtGamepad>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>

#include "../EmulatorMgr.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow &GetInstance()
    {
        // Static instance causes a segfault when closing the application, because
        // the QObjects get deleted after the QApplication, so use the old style
        // of Singleton instead, so we can control when the instance is destroyed.
        //static MainWindow instance;
        if (instance == NULL)
            instance = new MainWindow(NULL);
        return *instance;
    }
    static void DestroyInstance()
    {
        delete instance;
    }
    ~MainWindow();

    // Don't allow copy and assignment.
    MainWindow(const MainWindow&) = delete;
    void operator=(const MainWindow&) = delete;

protected:
    explicit MainWindow(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    void SetupMenuBar();
    void SetupStatusBar();
    void SetupGamepad();

    static void drawFrame(uint32_t *frameBuffer);

    QGraphicsView *graphicsView;
    QLabel *labelFps;
    QLabel *labelPause;

    bool paused;
    bool capFps;

    EmulatorMgr emulator;

    // FPS variables.
    QElapsedTimer fpsTimer;
    int frameCount;

    // Frame cap variables.
    static QElapsedTimer frameCapTimer;

    static MainWindow *instance;

    QGamepad *gamepad;
    QGamepadKeyNavigation *gamepadKeyNavigation;

private slots:
    void slotOpenRom();
    void slotTogglePause(bool checked);
    void slotToggleCapFps(bool checked);
    void slotQuit();
    void slotDrawFrame(uint32_t *frameBuffer);

signals:
    void frameReady(uint32_t *frameBuffer);
};
