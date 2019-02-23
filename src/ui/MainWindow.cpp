#include <QtWidgets/QtWidgets>
#include <stdint.h>
#include <thread>
#include "DebugWindow.h"
#include "MainWindow.h"
#include "QtFrameHandler.h"
#include "../EmulatorMgr.h"
#include "../Input.h"


const QHash<int, Buttons::Button> keymap {
    {Qt::Key_W, Buttons::Button::eButtonUp},
    {Qt::Key_S, Buttons::Button::eButtonDown},
    {Qt::Key_A, Buttons::Button::eButtonLeft},
    {Qt::Key_D, Buttons::Button::eButtonRight},
    {Qt::Key_H, Buttons::Button::eButtonSelect},
    {Qt::Key_J, Buttons::Button::eButtonStart},
    {Qt::Key_K, Buttons::Button::eButtonB},
    {Qt::Key_L, Buttons::Button::eButtonA}
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    graphicsView(NULL),
    labelFps(NULL),
    labelPause(NULL),
    paused(false),
    capFps(true),
    frameHandler(NULL),
    emulator(NULL),
    fpsTimer(),
    frameCount(0),
    frameCapTimer(),
    gamepad(NULL),
    gamepadKeyNavigation(NULL),
    debugWindow(NULL)
{
    SetupMenuBar();
    SetupStatusBar();
    SetupGamepad();

    graphicsView = new QGraphicsView(this);
    graphicsView->setFrameStyle(QFrame::NoFrame);
    setCentralWidget(graphicsView);

    QGraphicsScene *scene = new QGraphicsScene(this);
    graphicsView->setScene(scene);
    graphicsView->scale(5, 5);

    debugWindow = new DebugWindow(this);
    debugWindow->show();

    fpsTimer.start();
    frameCapTimer.start();

    connect(this, SIGNAL(SignalFrameReady()), this, SLOT(slotDrawFrame()));
    connect(this, SIGNAL(SignalShowMessageBox(const QString&)), this, SLOT(slotShowMessageBox(const QString &)));

    frameHandler = new QtFrameHandler(this);
    emulator = new EmulatorMgr(frameHandler, debugWindow);

    if (qApp->arguments().size() >= 2)
    {
        QString filename = qApp->arguments().at(1);
        emulator->LoadRom(filename.toLatin1().data());
    }

    QSettings settings;
    restoreGeometry(settings.value("MainWindowGeometry").toByteArray());
}


MainWindow::~MainWindow()
{
    delete emulator;
    delete frameHandler;
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    Buttons::Button button = keymap.value(event->key(), Buttons::Button::eButtonNone);
    if (button != Buttons::Button::eButtonNone)
    {
        emulator->ButtonPressed(button);
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}


void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    Buttons::Button button = keymap.value(event->key(), Buttons::Button::eButtonNone);
    if (button != Buttons::Button::eButtonNone)
    {
        emulator->ButtonReleased(button);
    }
    else
    {
        QMainWindow::keyReleaseEvent(event);
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    debugWindow->close();

    QSettings settings;
    settings.setValue("MainWindowGeometry", saveGeometry());
    QWidget::closeEvent(event);
}


void MainWindow::SetupMenuBar()
{
    menuBar()->setNativeMenuBar(false);

    // File Menu
    QMenu *fileMenu = menuBar()->addMenu("&File");

    // File | Open...
    QAction *fileOpen = new QAction("&Open ROM...", this);
    fileMenu->addAction(fileOpen);
    connect(fileOpen, SIGNAL(triggered()), this, SLOT(slotOpenRom()));

    // File | Open Recent
    QMenu *fileOpenRecentMenu = fileMenu->addMenu("Open &Recent");
    for (int i = 0; i < MAX_RECENT_FILES; i++)
    {
        recentFilesActions[i] = new QAction(this);
        recentFilesActions[i]->setVisible(false);
        fileOpenRecentMenu->addAction(recentFilesActions[i]);
        connect(recentFilesActions[i], SIGNAL(triggered()), this, SLOT(slotOpenRecentRom()));
    }
    UpdateRecentFilesActions();

    // File | Quit
    fileMenu->addSeparator();
    QAction *fileQuit = new QAction("&Quit", this);
    fileMenu->addAction(fileQuit);
    connect(fileQuit, SIGNAL(triggered()), this, SLOT(slotQuit()));

    // Emulator Menu
    QMenu *emuMenu = menuBar()->addMenu("&Emulator");

    // Emulator | Reset
    QAction *emuReset = new QAction("&Reset", this);
    emuMenu->addAction(emuReset);
    connect(emuReset, SIGNAL(triggered()), this, SLOT(slotReset()));

    // Emulator | Pause
    QAction *emuPause = new QAction("&Pause", this);
    emuPause->setShortcut(Qt::Key_Escape);
    emuPause->setCheckable(true);
    emuMenu->addAction(emuPause);
    connect(emuPause, SIGNAL(triggered(bool)), this, SLOT(slotTogglePause(bool)));

    // Emulator | End
    QAction *emuEnd = new QAction("&End Emulation", this);
    emuMenu->addAction(emuEnd);
    connect(emuEnd, SIGNAL(triggered()), this, SLOT(slotEndEmulation()));

    // Emulator | Cap FPS
    QAction *emuCapFps = new QAction("&Cap FPS", this);
    emuCapFps->setShortcut(Qt::Key_F10);
    emuCapFps->setCheckable(true);
    emuCapFps->setChecked(true);
    emuMenu->addAction(emuCapFps);
    connect(emuCapFps, SIGNAL(triggered(bool)), this, SLOT(slotToggleCapFps(bool)));
}


void MainWindow::SetupStatusBar()
{
    labelFps = new QLabel("0 FPS", this);
    labelPause = new QLabel("", this);
    statusBar()->addPermanentWidget(labelFps);
    statusBar()->addPermanentWidget(labelPause);
}


void MainWindow::SetupGamepad()
{
    QList<int> gamepads = QGamepadManager::instance()->connectedGamepads();
    if (gamepads.isEmpty())
    {
        return;
    }

    gamepad = new QGamepad(*gamepads.begin(), this);
    gamepadKeyNavigation = new QGamepadKeyNavigation(this);

    gamepadKeyNavigation->setGamepad(gamepad);
    gamepadKeyNavigation->setUpKey(Qt::Key_W);
    gamepadKeyNavigation->setDownKey(Qt::Key_S);
    gamepadKeyNavigation->setLeftKey(Qt::Key_A);
    gamepadKeyNavigation->setRightKey(Qt::Key_D);
    gamepadKeyNavigation->setButtonSelectKey(Qt::Key_H);
    gamepadKeyNavigation->setButtonStartKey(Qt::Key_J);
    gamepadKeyNavigation->setButtonXKey(Qt::Key_K);
    gamepadKeyNavigation->setButtonAKey(Qt::Key_L);
}


void MainWindow::UpdateRecentFile(const QString &filename)
{
    QSettings settings;
    QStringList files = settings.value("RecentFileList").toStringList();

    files.removeAll(filename);
    files.prepend(filename);
    while (files.size() > MAX_RECENT_FILES)
        files.removeLast();

    settings.setValue("RecentFileList", files);

    UpdateRecentFilesActions();
}


void MainWindow::UpdateRecentFilesActions()
{
    QSettings settings;
    QStringList files = settings.value("RecentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), MAX_RECENT_FILES);

    for (int i = 0; i < numRecentFiles; i++) {
        QString text = tr("&%1 %2").arg(i).arg(files[i]);
        recentFilesActions[i]->setText(text);
        recentFilesActions[i]->setData(files[i]);
        recentFilesActions[i]->setVisible(true);
    }
    for (int i = numRecentFiles; i < MAX_RECENT_FILES; i++)
        recentFilesActions[i]->setVisible(false);
}


void MainWindow::FrameReady(uint32_t *displayFrameBuffer)
{
    // This function runs in the thread context of the Emulator worker thread.

    // Copy data so Emulator thread doesn't change data while we're drawing the screen.
    memcpy(frameBuffer, displayFrameBuffer, sizeof(frameBuffer));

    // Signal the main thread to draw the screen.
    emit SignalFrameReady();

    uint64_t elapsedTime = frameCapTimer.elapsed();

    const float frameMillis = 1.0 / 60 * 1000;
    if (capFps && elapsedTime < frameMillis)
    {
        // Block the Emulator from running to limit frame rate.
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(frameMillis - elapsedTime)));
    }

    frameCapTimer.restart();
}


void MainWindow::RequestMessageBox(const std::string &message)
{
    // This function runs in the thread context of the Emulator worker thread.
    // Signal the main thread to show a message box.
    emit SignalShowMessageBox(QString::fromStdString(message));
}


void MainWindow::slotOpenRom()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open ROM File", "./data");
    QString message = "filename = ";
    statusBar()->showMessage(message + filename, 5000);

    UpdateRecentFile(filename);

    emulator->LoadRom(filename.toLatin1().data());
}


void MainWindow::slotOpenRecentRom()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString filename = action->data().toString();
        statusBar()->showMessage("Filename = " + filename, 5000);

        UpdateRecentFile(filename);

        emulator->LoadRom(filename.toLatin1().data());
    }
}


void MainWindow::slotReset()
{
    emulator->ResetEmulation();
}


void MainWindow::slotTogglePause(bool checked)
{
    paused = checked;
    labelPause->setText(checked ? "Paused" : "");
    emulator->PauseEmulation(paused);
}


void MainWindow::slotEndEmulation()
{
    emulator->EndEmulation();
    graphicsView->scene()->clear();
}


void MainWindow::slotToggleCapFps(bool checked)
{
    capFps = checked;
}


void MainWindow::slotQuit()
{
    emulator->EndEmulation();
    qApp->quit();
}


void MainWindow::slotDrawFrame()
{
    uint64_t elapsedTime = fpsTimer.elapsed();

    if (elapsedTime > 1000)
    {
        int fps = frameCount / (elapsedTime / 1000.0);
        labelFps->setText(QString::number(fps) + " FPS");
        fpsTimer.restart();
        frameCount = 0;
    }
    else
    {
        frameCount++;
    }

    QImage img((uchar *)(frameBuffer), 160, 144, QImage::Format_RGB32);
    graphicsView->scene()->clear();
    graphicsView->scene()->addPixmap(QPixmap::fromImage(img));

    debugWindow->DrawFrame();
}


void MainWindow::slotShowMessageBox(const QString &message)
{
    QMessageBox msg;
    msg.setText(message);
    msg.exec();
}