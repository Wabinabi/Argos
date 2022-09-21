#include "tripdata.h"
#include "ui_tripdata.h"
#include "argous.h"
#include "datamodel.h"

#include <QApplication>

Argous *homePage;

TripData::TripData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TripData)
{
    ui->setupUi(this);
}

TripData::~TripData()
{
    delete ui;
}

void TripData::on_HomeBtn_clicked()
{
    parentWidget()->show();
    close();


}

void TripData::closeEvent (QCloseEvent *event)
{
    parentWidget()->show();
}

void TripData::on_DisplayModel_clicked()
{
    DataModel dataModel;
    dataModel.setModal(false); //takes arguement for True/False, this determines whether the previous window can be accessed while the popup is open
    dataModel.exec();

}

