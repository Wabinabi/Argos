#ifndef ACTIONS_H
#define ACTIONS_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include <QVector>
#include <QFileDialog>
#include <QList>
#include <QtGui>
#include <QCloseEvent>
#include <QScrollArea>
#include <QDebug>
#include <iostream>

#include <QShortcut>

class QMenu;
class QAction;

class Actions
{
public:
    Actions();
    void createHelpAction();

private:
    void help();

    QAction *helpAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
};


#endif // ACTIONS_H
