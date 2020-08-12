#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);
    qRegisterMetaType<QVector<float>>();

    MainWindow w;
    w.showMaximized();
    return a.exec();
}
