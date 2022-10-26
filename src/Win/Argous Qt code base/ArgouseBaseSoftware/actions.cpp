#include "actions.h"

Actions::Actions()
{

}



void Actions::createHelpAction()
{
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));
}

void Actions::help()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("../ArgouseBaseSoftware/appdata/docs/html/index.html"));
}
