#include "mainwindow.h"
#include "ReminderWidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    ReminderWidget rw;
    rw.show();
    return a.exec();
}
