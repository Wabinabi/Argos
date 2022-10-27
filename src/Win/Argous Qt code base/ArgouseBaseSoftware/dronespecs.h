/****************************************************
 * Description: Drone Specs header file for dronespecs.cpp
 * Author/s: Jimmy, Monique
****************************************************/

#ifndef DRONESPECS_H
#define DRONESPECS_H

#include <QDialog>
#include "ui_dronespecs.h"

/***************************************************/

namespace Ui {
class DroneSpecs;
}

class DroneSpecs : public QDialog
{
    Q_OBJECT

public:
    explicit DroneSpecs(QWidget *parent = nullptr);
    ~DroneSpecs();

private:
    Ui::DroneSpecs *ui;
};

#endif // DRONESPECS_H
