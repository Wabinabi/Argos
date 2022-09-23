#include "homepage.h"
#include "ui_homepage.h"


HomePage::HomePage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomePage)
{
    ui->setupUi(this);
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::on_pushButton_clicked()
{
    TripData tripData;
    tripData.setModal(false); //takes arguement for True/False, this determines whether the previous window can be accessed while the popup is open
    tripData.exec();
}

