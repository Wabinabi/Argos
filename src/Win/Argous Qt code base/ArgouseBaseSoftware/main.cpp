#include <QApplication>
#include "homepage.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Argous Base Software");

    HomePage homePage;
    homePage.show();

    return a.exec();
}
