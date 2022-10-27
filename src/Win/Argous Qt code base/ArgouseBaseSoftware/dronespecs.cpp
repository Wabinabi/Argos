/****************************************************
 * Description: Drone Specs page
 * Author/s: Jimmy, Monique
****************************************************/

#include "dronespecs.h"

/***************************************************/

DroneSpecs::DroneSpecs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DroneSpecs)
{
    ui->setupUi(this);
}

DroneSpecs::~DroneSpecs()
{
    delete ui;
}
