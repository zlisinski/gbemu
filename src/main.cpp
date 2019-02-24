#include <QtWidgets/QApplication>
#include "ui/MainWindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("zlisinski");
    app.setApplicationName("gbemu");

    MainWindow window;
    window.setWindowTitle("GBEmu");
    window.show();

    return app.exec();
}