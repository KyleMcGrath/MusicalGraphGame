// Produced by Kyle McGrath in 2018
// All rights reserved.

#include "include/view/main/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();

    return a.exec();
}
