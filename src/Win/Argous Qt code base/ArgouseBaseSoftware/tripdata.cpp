/****************************************************
 * Description: TripData connects to the tripdata.ui page.
 * This page depicts overall trip data, 2D trends, and an events log.
 * It provides the user with an overall image of how the imported flight performed.
 * This page is opened from the homepage. From it, the users can access the 3D model.
 *
 * Author/s: Monique Kuhn, Jimmy Trac
****************************************************/

#include "tripdata.h"
#include "ui_tripdata.h"
#include "datamodel.h"

#include <QApplication>
#include <QMainWindow>

/***************************************************/

HomePage *homePage;

TripData::TripData(QWidget *parent,
       QVector<HomePage::DroneEvent> *emergencyEvents,
       QVector<HomePage::DroneEvent> *verboseEvents,
       QVector<HomePage::DroneEvent> *informEvents,
       HomePage::DroneSeriesData *altitude,
       HomePage::DroneSeriesData *temperature,
       HomePage::DroneSeriesData *throttle) :
        QDialog(parent),
        ui(new Ui::TripData),
        isTouching(false)
{
    ui->setupUi(this);

    /*F1 help shortcut - consider making F1 a global feature*/
    QShortcut *shortcut = new QShortcut(QKeySequence(QKeySequence::HelpContents),this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(help()));

    this->setWindowTitle(QStringLiteral("Drone Trip Data"));
    void passData();

    /*Getter/Setters for events and trends data*/
    locEmergencyEvents = *emergencyEvents;
    locVerboseEvents = *verboseEvents;
    locInformEvents = *informEvents;

    locThrottle = *throttle;
    locTemperature = *temperature;
    locAltitude = *altitude;

    /*Populate widget with data*/
    drawStackedTrends();
    readDroneStats();
    //readDroneEvents();

}


TripData::~TripData()
{
    delete ui;
}

/***************************************************/

void TripData::drawStackedTrends(){
    /*Indexes for the stacked widget*/
    tempIndex = 0;
    throttleIndex = 1;
    altitudeIndex = 2;
    int defaultIndex = 0;

    drawXYSeries(tempIndex, locTemperature, tempChart, tempChartView);
    drawXYSeries(throttleIndex, locThrottle, throttleChart, throttleChartView);
    drawXYSeries(altitudeIndex, locAltitude, altitudeChart, altitudeChartView);

    ui->stackedWidget->setCurrentIndex(defaultIndex);
    highlightTrendButton(defaultIndex);
}

void TripData::highlightTrendButton(int _index){
    switch (_index)  {
        case 0:
            ui->TempBtn->setStyleSheet("background-color: #FF752B");
            ui->ThrottleBtn->setStyleSheet("background-color: ");
            ui->AltitudeBtn->setStyleSheet("background-color: ");
            break;

        case 1:
            ui->TempBtn->setStyleSheet("background-color: ");
            ui->ThrottleBtn->setStyleSheet("background-color: #FF752B");
            ui->AltitudeBtn->setStyleSheet("background-color: ");
            break;

        case 2:
        ui->TempBtn->setStyleSheet("background-color: ");
        ui->ThrottleBtn->setStyleSheet("background-color: ");
        ui->AltitudeBtn->setStyleSheet("background-color: #FF752B");
        break;

        default:
            ui->ThrottleBtn->setStyleSheet("background-color: ");
            ui->TempBtn->setStyleSheet("background-color: ");
            ui->AltitudeBtn->setStyleSheet("background-color: ");
    }
}

void TripData::drawXYSeries(int stackIndex, HomePage::DroneSeriesData droneData, QChart *selChart, QChartView *selChartView){
    QLineSeries *XYSeries = new QLineSeries();
    for (int i = 0; i < droneData.data.size(); ++i) {
        XYSeries->append(droneData.data[i].time/1000, droneData.data[i].y);
    }

    selChart->addSeries(XYSeries);
    selChart->legend()->hide();

    //X Axis
    QValueAxis *axisX = new QValueAxis;

    axisX->setTitleText("Time (s)");
    selChart->addAxis(axisX, Qt::AlignBottom);
    XYSeries->attachAxis(axisX);

    //Y Axis
    QValueAxis *axisY = new QValueAxis;

    if (ui->stackedWidget->currentIndex() == 3){
        axisY->setTitleText("Height above ground (cm)");
    }

    if (ui->stackedWidget->currentIndex() == 2){
        axisY->setTitleText("Throttle (x/2056)");
    }

    if (ui->stackedWidget->currentIndex() == 1){
        axisY->setTitleText("Temperature (deg c)");
    }

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

    m_scatter2->setName("Events");
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


void TripData::on_EventsBtn_clicked()
{
    ui->ThrottleBtn->setStyleSheet("background-color: ");
    ui->TempBtn->setStyleSheet("background-color: ");
    ui->AltitudeBtn->setStyleSheet("background-color: ");

    ui->stackedWidget->setCurrentIndex(eventsIndex);
    currentChart = eventsChart;
}

void TripData::on_ThrottleBtn_clicked()
{
    highlightTrendButton(throttleIndex);
    ui->stackedWidget->setCurrentIndex(throttleIndex);
    currentChart = throttleChart;
}

void TripData::on_TempBtn_clicked()
{
    highlightTrendButton(tempIndex);
    ui->stackedWidget->setCurrentIndex(tempIndex);
    currentChart = tempChart;
}

void TripData::on_AltitudeBtn_clicked()
{
    highlightTrendButton(altitudeIndex);
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
    QString line, htmlLine, textBlock;

    QStringList tokens;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){

            line = stream.readLine();
            tokens = line.split(QRegularExpression(":"));
            htmlLine = "<html><b>" + tokens[0] + ": </b></html>" + tokens[1] + "<html><br></html>";

            textBlock.append(htmlLine);
        }

        ui->droneStats->setText(textBlock);
    }
    file.close();
}

void TripData::help()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("../ArgouseBaseSoftware/appdata/docs/html/index.html"));
}



