#include <QtWidgets/QApplication>
#include "MainWindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("zlisinski");
    app.setApplicationName("zlgb");

    MainWindow window;
    window.setWindowTitle("ZLGB");
    window.show();

    return app.exec();
}