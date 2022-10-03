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
    m_isTouching(false),
    ui(new Ui::TripData)
{
    ui->setupUi(this);

    QVector<QString> testData;


    //setRubberBand(QChartView::RectangleRubberBand);


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


    readDroneStats();


    //probs need to process test data with a sim function

    drawEventsPlot();
    drawXYSeries(tempIndex, locTemperature, tempChart, tempChartView);
    drawXYSeries(throttleIndex, locThrottle, throttleChart, throttleChartView);
    drawXYSeries(altitudeIndex, locAltitude, altitudeChart, altitudeChartView);

    ui->stackedWidget->setCurrentIndex(0);
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

void TripData::drawXYSeries(int stackIndex, HomePage::DroneSeriesData droneData, QChart *selChart, QChartView *selChartView){
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
    //selChart = new QChart();
    selChart->addSeries(XYSeries);
    selChart->legend()->hide();
    //chart->setTitle("Test Chart");

    //X Axis
    QValueAxis *axisX = new QValueAxis;
    //axisX->setTickCount(10);
    //axisX->setFormat("hh:mm");
    //axisX->setTitleText("Date");
    selChart->addAxis(axisX, Qt::AlignBottom);
    XYSeries->attachAxis(axisX);

    //Y Axis
    QValueAxis *axisY = new QValueAxis;
    //axisY->setLabelFormat("%i");
    //axisY->setTitleText("Temp Data");
    selChart->addAxis(axisY, Qt::AlignLeft);
    XYSeries->attachAxis(axisY);

    selChartView->setRenderHint(QPainter::Antialiasing);
    ui->stackedWidget->insertWidget(stackIndex, selChartView);
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
    eventsChartView = new QChartView(chart);
    eventsChartView->setRenderHint(QPainter::Antialiasing);
    //chart->addSeries(m_scatter3);
    chart->addSeries(m_scatter2);
    chart->addSeries(m_scatter);
    chart->createDefaultAxes();
    //update dissss
    //chart->axes(Qt::Horizontal).first()->setRange(0, 200);
    chart->axes(Qt::Vertical).first()->setRange(0, 4.5);

    ui->stackedWidget->insertWidget(eventsIndex, eventsChartView);
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

    ui->ThrottleBtn->setFlat(true);
    ui->EventsBtn->setFlat(false);
    ui->TempBtn->setFlat(false);
    ui->AltitudeBtn->setFlat(false);

    ui->stackedWidget->setCurrentIndex(throttleIndex);
    currentChart = throttleChart;
}

void TripData::on_EventsBtn_clicked()
{
    ui->ThrottleBtn->setFlat(false);
    ui->EventsBtn->setFlat(true);
    ui->TempBtn->setFlat(false);
    ui->AltitudeBtn->setFlat(false);

    ui->stackedWidget->setCurrentIndex(eventsIndex);
    currentChart = eventsChart;
}

void TripData::on_TempBtn_clicked()
{
    ui->ThrottleBtn->setFlat(false);
    ui->EventsBtn->setFlat(false);
    ui->TempBtn->setFlat(true);
    ui->AltitudeBtn->setFlat(false);

    ui->stackedWidget->setCurrentIndex(tempIndex);
    currentChart = tempChart;
}

void TripData::on_AltitudeBtn_clicked()
{
    ui->ThrottleBtn->setFlat(false);
    ui->EventsBtn->setFlat(false);
    ui->TempBtn->setFlat(false);
    ui->AltitudeBtn->setFlat(true);

    ui->stackedWidget->setCurrentIndex(altitudeIndex);
    currentChart = altitudeChart;
}

void TripData::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        currentChart->zoomIn();
        break;
    case Qt::Key_Minus:
        currentChart->zoomOut();
        break;
    case Qt::Key_Left:
        currentChart->scroll(-100, 0);
        break;
    case Qt::Key_Right:
        currentChart->scroll(100, 0);
        break;
    case Qt::Key_Up:
        currentChart->scroll(0, 100);
        break;
    case Qt::Key_Down:
        currentChart->scroll(0, -100);
        break;
    default:
        break;
    }
}

void TripData::readDroneStats(){
    QFile file("../ArgouseBaseSoftware/appdata/tripStats.txt");
    QString line;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){

            line.append(stream.readLine()+"\n");
        }
        ui->droneStats->setText(line);
    }
    file.close();
}


