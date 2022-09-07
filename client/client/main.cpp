#include "mainwindowclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowClient w;
    w.show();
    return a.exec();
}
