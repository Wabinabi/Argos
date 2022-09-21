#include "tripdetails.h"
#include "ui_tripdetails.h"

TripDetails::TripDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TripDetails)
{
    ui->setupUi(this);
}

TripDetails::~TripDetails()
{
    delete ui;
}


