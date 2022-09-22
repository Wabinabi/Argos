#include <QApplication>
#include "homepage.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    HomePage homePage;
    homePage.show();

    return a.exec();
}
