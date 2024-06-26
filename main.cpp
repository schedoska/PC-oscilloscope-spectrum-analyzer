#include "mainwindow.h"

#include <QApplication>
#include <cmath>
#include <QDebug>
#include <QRandomGenerator>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const QString translationFileName = "test_newQT3_en.qm";

    QTranslator translator = QTranslator();
    if(translator.load(":/test_newQT3_en.qm"))
        a.installTranslator(&translator);
    else
        qDebug() << "Nie udało się załadować pliku: " + translationFileName;

    a.removeTranslator(&translator);


    MainWindow w;
    w.show();
    return a.exec();
}
