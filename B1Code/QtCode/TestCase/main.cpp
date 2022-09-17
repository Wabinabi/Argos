//#include "mainwindow.h"
#include "datatranslator.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QString data = "E:/Uni/00-Final Year Project/Argous/B1Code/TestCase.csv";
    QString dest = data;
    //QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    //return a.exec();
    DataTranslator test = DataTranslator();
    test.SetFilePath(data, dest);
    qDebug() << test.GenerateFile();
    return 0;
}
