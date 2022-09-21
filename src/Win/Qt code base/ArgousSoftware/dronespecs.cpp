#include "dronespecs.h"
#include "ui_dronespecs.h"

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
