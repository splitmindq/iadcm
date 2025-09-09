#include "lib/mainwindow.h"

#include <QApplication>
#include <windows.h>

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(CP_UTF8);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}