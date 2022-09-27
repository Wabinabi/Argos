#include <QApplication>
#include "homepage.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Argous Base Software");

    HomePage homePage;
    homePage.show();

    #if defined(Q_OS_SYMBIAN)
         homePage.showMaximized();
     #else
         homePage.show();
     #endif

    return a.exec();
}
