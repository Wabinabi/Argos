#include <QApplication>
#include <QFile>

#include "homepage.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Set the app stlye sheet
    QFile styleSheetFile("../ArgouseBaseSoftware/Diffnes.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleSheetFile.readAll());
    a.setStyleSheet(styleSheet);

    a.setApplicationName("Argous Base Software");

    HomePage homePage;
    homePage.show();

    return a.exec();
}
