#include "tripdata.h"
#include "ui_tripdata.h"

#include "datamodel.h"

#include <QApplication>
#include <QMainWindow>

HomePage *homePage;


TripData::TripData(QWidget *parent,
                   QVector<HomePage::DroneEvent> *emergencyEvents,
                   QVector<HomePage::DroneEvent> *verboseEvents,
                   QVector<HomePage::DroneEvent> *informEvents,
                   HomePage::DroneSeriesData *altitude,
                   HomePage::DroneSeriesData *temperature,
                   HomePage::DroneSeriesData *throttle) :
    QDialog(parent),
    ui(new Ui::TripData)
{
    ui->setupUi(this);

    QVector<QString> testData;

    void passData();

    locEmergencyEvents = *emergencyEvents;
    locVerboseEvents = *verboseEvents;
    locInformEvents = *informEvents;


    locThrottle = *throttle;
    locTemperature = *temperature;
    locAltitude = *altitude;

    eventsIndex = 0;
    tempIndex = 1;
    throttleIndex = 2;
    altitudeIndex = 3;
    //this->setParent( parent );



    ui->stackedWidget->setCurrentIndex(1);


    //probs need to process test data with a sim function

    drawEventsPlot();
    drawXYSeries(tempIndex, locTemperature);
    drawXYSeries(throttleIndex, locThrottle);
    drawXYSeries(altitudeIndex, locAltitude);

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

void TripData::drawXYSeries(int stackIndex, HomePage::DroneSeriesData droneData){
    QLineSeries *XYSeries = new QLineSeries();
    //QFile File("../ArgouseBaseSoftware/test2DPlot.txt");

    //if (File.open(QIODevice::ReadOnly | QIODevice::Text)){
        //QTextStream stream(&File);
        //while (!stream.atEnd()) {
            //QString line = stream.readLine();
            //if (line.startsWith("#") || line.startsWith(":"))
            //    continue;

    for (int i = 0; i < droneData.data.size(); ++i) {
        XYSeries->append(droneData.data[i].time, droneData.data[i].y);
    }
//            QStringList values = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
//            QDateTime momentInTime;
//            momentInTime.setTime(QTime(values[0].toInt(), values[1].toInt()));
//            XYSeries->append(momentInTime.toMSecsSinceEpoch(), values[2].toDouble());
//        }

    //File.close();

    //QFile file("../ArgouseBaseSoftware/droneStats.txt");

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
    //QMainWindow window;

//    window.setCentralWidget(chartView);
//    window.resize(820, 600);
//    window.show();

    //ui->graphDisp->addWidget(chartView);
    ui->stackedWidget->insertWidget(stackIndex, chartView);
}

void TripData::drawEventsPlot(){
    //chart->setTitle("Click to interact with scatter points");

    m_scatter = new QScatterSeries();
    m_scatter2 = new QScatterSeries();

    // For each drone events timeline, create a scatter plot
    // Events in the same timeline should stay on the same row
    //

    m_scatter->setName("Emergencies");
    //plot emergency events on row 1
    for (int i = 0; i < locEmergencyEvents.size(); i++){
        int x = locEmergencyEvents[i].time;
        *m_scatter << QPointF(x, 1);
    }

//    m_scatter2->setName("Verbose");
//    //plot verbose events on row 2
//    for (int i = 0; i < locVerboseEvents.size(); i++){
//        int x = locVerboseEvents[i].time;
//        *m_scatter << QPointF(x, 2);
//    }

    m_scatter2->setName("Informative");
    //plot informative events on row 3
    for (int i = 0; i < locInformEvents.size(); i++){
        int x = locInformEvents[i].time;
        *m_scatter2 << QPointF(x, 3);
    }

    QChart *chart = new QChart();
    eventsChart = new QChartView(chart);
    eventsChart->setRenderHint(QPainter::Antialiasing);
    //chart->addSeries(m_scatter3);
    chart->addSeries(m_scatter2);
    chart->addSeries(m_scatter);
    chart->createDefaultAxes();
    //update dissss
    //chart->axes(Qt::Horizontal).first()->setRange(0, 200);
    chart->axes(Qt::Vertical).first()->setRange(0, 4.5);

    ui->stackedWidget->insertWidget(eventsIndex, eventsChart);
    //connect(m_scatter, &QScatterSeries::clicked, this, &TripData::handleClickedPoint);
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

void TripData::handleClickedPoint(const QPointF &point)
{
    QPointF clickedPoint = point;
    // Find the closest point from series 1
    QPointF closest(INT_MAX, INT_MAX);
    qreal distance(INT_MAX);
    const auto points = m_scatter->points();
    for (const QPointF &currentPoint : points) {
        qreal currentDistance = qSqrt((currentPoint.x() - clickedPoint.x())
                                      * (currentPoint.x() - clickedPoint.x())
                                      + (currentPoint.y() - clickedPoint.y())
                                      * (currentPoint.y() - clickedPoint.y()));
        if (currentDistance < distance) {
            distance = currentDistance;
            closest = currentPoint;
        }
    }

    // Remove the closes point from series 1 and append it to series 2
    m_scatter->remove(closest);
    m_scatter2->append(closest);
}

void TripData::on_ThrottleBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(throttleIndex);
}

void TripData::on_EventsBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(eventsIndex);
}

void TripData::on_TempBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(tempIndex);
}

void TripData::on_AltitudeBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(altitudeIndex);
}

