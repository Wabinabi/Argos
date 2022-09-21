#include "argous.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Argous w;
    w.show();
    return a.exec();

    a.setQuitOnLastWindowClosed(false);
}
