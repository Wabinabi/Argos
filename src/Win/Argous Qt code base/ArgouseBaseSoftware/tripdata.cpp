#include "tripdata.h"
#include "ui_tripdata.h"
#include "homepage.h"
#include "datamodel.h"

#include <QApplication>
#include <QMainWindow>

HomePage *homePage;

TripData::TripData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TripData)
{
    ui->setupUi(this);

    QVector<QString> testData;



    //probs need to process test data with a sim function

    drawXYSeries();
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
//QLineSeries TripData::reshapeVector(QVector<QString> XYData){


//}
//QLineSeries *XYSeries

void TripData::drawXYSeries(){
    QLineSeries *XYSeries = new QLineSeries();
    QFile File("../ArgouseBaseSoftware/test2DPlot.txt");

    if (File.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&File);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.startsWith("#") || line.startsWith(":"))
                continue;
            QStringList values = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            QDateTime momentInTime;
            momentInTime.setTime(QTime(values[0].toInt(), values[1].toInt()));
            XYSeries->append(momentInTime.toMSecsSinceEpoch(), values[2].toDouble());
        }
    }
    File.close();

    QFile file("../ArgouseBaseSoftware/droneStats.txt");

    //needs to know the file that it's formatting
    //process the same formatted data file
        //Parent.vector.time and ..vector.value is already available for all of these
    //QStringList values = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    //QDateTime momentInTime;
    //momentInTime.setDate(QDate(values[0].toInt(), values[1].toInt() , 15));
    //XYSeries->append(momentInTime.toMSecsSinceEpoch(), values[2].toDouble());

    //generate a Qchart
    QChart *chart = new QChart();
    chart->addSeries(XYSeries);
    chart->legend()->hide();
    //chart->setTitle("Test Chart");

    //X Axis
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("hh:mm");
    //axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    XYSeries->attachAxis(axisX);

    //Y Axis
    QValueAxis *axisY = new QValueAxis;
    //axisY->setLabelFormat("%i");
    //axisY->setTitleText("Temp Data");
    chart->addAxis(axisY, Qt::AlignLeft);
    XYSeries->attachAxis(axisY);


    //Chart View
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    //Opens a window with the graph
    QMainWindow window;

//    window.setCentralWidget(chartView);
//    window.resize(820, 600);
//    window.show();

    ui->graphDisp->addWidget(chartView);
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

