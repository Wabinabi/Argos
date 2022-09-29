#include "tripdata.h"
#include "ui_tripdata.h"
#include "homepage.h"
#include "datamodel.h"

#include <QApplication>

HomePage *homePage;

TripData::TripData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TripData)
{
    ui->setupUi(this);

    // QLineSeries *series = new QLineSeries();
    // function that opens the data:
    //     QFile sunSpots(":sun");
//    if (!sunSpots.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        return 1;
//    }
}

TripData::~TripData()
{
    delete ui;
}

//YOU'LL NEED TO CHANGER THJE STRUHYJCT
QLineSeries TripData::reshapeVector(QVector<QString> XYData){

}

void TripData::drawXYData(QVector<QString> XYData){
    //needs to know the file that it's formatting
    //process the same formatted data file
        //Parent.vector.time and ..vector.value is already available for all of these
        //QStringList values = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        //QDateTime momentInTime;
        //momentInTime.setDate(QDate(values[0].toInt(), values[1].toInt() , 15));
        //series->append(momentInTime.toMSecsSinceEpoch(), values[2].toDouble());

    //generate a Qchart
        //QChart *chart = new QChart();
        //chart->addSeries(series);
        //chart->legend()->hide();
        //chart->setTitle("Sunspots count (by Space Weather Prediction Center)");

    //X Axis
        //QDateTimeAxis *axisX = new QDateTimeAxis;
        //axisX->setTickCount(10);
        //axisX->setFormat("MMM yyyy");
        //axisX->setTitleText("Date");
        //chart->addAxis(axisX, Qt::AlignBottom);
        //series->attachAxis(axisX);

    //Y Axis
        //QValueAxis *axisY = new QValueAxis;
        //axisY->setLabelFormat("%i");
        //axisY->setTitleText("Sunspots count");
        //chart->addAxis(axisY, Qt::AlignLeft);
        //series->attachAxis(axisY);


    //Chart View
        //QChartView *chartView = new QChartView(chart);
        //chartView->setRenderHint(QPainter::Antialiasing);

    //Opens a window with the graph
        //QMainWindow window;
        //window.setCentralWidget(chartView);
        //window.resize(820, 600);
        //window.show();

    //draws it all in the same "location"widget location"
}

void TripData::on_HomeBtn_clicked()
{
    parentWidget()->show();
    close();
}

//void TripData::closeEvent (QCloseEvent *event)
//{
//    parentWidget()->show();
//}

void TripData::on_DisplayModel_clicked()
{
    DataModel dataModel;
    dataModel.setModal(false); //takes arguement for True/False, this determines whether the previous window can be accessed while the popup is open
    dataModel.exec();
}

