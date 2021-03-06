#include "mainwindow.h"
#include "additionalwindow.h"

#include <QApplication>
#include <QLabel>
#include <QWidget>
#include <QLibraryInfo>
#include <QTranslator>

int main(int argc, char *argv[])
{  
    QApplication a(argc, argv);
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);
    MainWindow w;
    w.show();
    return a.exec();
}
