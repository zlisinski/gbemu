#include <QtWidgets/QApplication>
#include "ui/MainWindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow &window = MainWindow::GetInstance();

    window.resize(800, 773);
    window.setWindowTitle("GBEmu");
    window.show();

    int ret = app.exec();

    window.DestroyInstance();

    return ret;
}