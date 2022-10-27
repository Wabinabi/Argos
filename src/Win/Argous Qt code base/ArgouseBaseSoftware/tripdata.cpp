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

TripData::TripData(QWidget *_parent,
       QVector<HomePage::DroneEvent> *_emergencyEvents,
       QVector<HomePage::DroneEvent> *_verboseEvents,
       QVector<HomePage::DroneEvent> *_informEvents,
       HomePage::DroneSeriesData *_altitude,
       HomePage::DroneSeriesData *_temperature,
       HomePage::DroneSeriesData *_throttle) :
        QDialog(_parent),
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
    locEmergencyEvents = *_emergencyEvents;
    locVerboseEvents = *_verboseEvents;
    locInformEvents = *_informEvents;

    locThrottle = *_throttle;
    locTemperature = *_temperature;
    locAltitude = *_altitude;

    /*Populate widget with data*/
    drawStackedTrends();
    displayDroneStats();
    displayDroneEvents();

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

    /*Disable trend button if it's data is not available*/
    /*Throttle*/
    if (locThrottle.data.size() == 0) {
        ui->ThrottleBtn->setEnabled(false);
        ui->ThrottleBtn->setText("Throttle not Enabled");
    } else {
        ui->ThrottleBtn->setEnabled(true);
        ui->ThrottleBtn->setText("Display Throttle Trend");
    }

    /*Altitude*/
    if (locThrottle.data.size() == 0) {
        ui->AltitudeBtn->setEnabled(false);
        ui->AltitudeBtn->setText("Altitude not Enabled");
    } else {
        ui->AltitudeBtn->setEnabled(true);
        ui->AltitudeBtn->setText("Display Altitude Trend");
    }

    /*Temperature*/
    if (locThrottle.data.size() == 0) {
        ui->TempBtn->setEnabled(false);
        ui->TempBtn->setText("Temperature not Enabled");
    } else {
        ui->TempBtn->setEnabled(true);
        ui->TempBtn->setText("Display Temperature Trend");
    }


    /*Set the default trend*/
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

    if (stackIndex == 0){axisY->setTitleText("Temperature (deg c)");}

    if (stackIndex == 1 ){axisY->setTitleText("Throttle (x/2056)");}

    if (stackIndex == 2){axisY->setTitleText("Height above ground (cm)");}


    selChart->addAxis(axisY, Qt::AlignLeft);
    XYSeries->attachAxis(axisY);

    selChartView->setRenderHint(QPainter::Antialiasing);
    ui->stackedWidget->insertWidget(stackIndex, selChartView);
}

void TripData::displayDroneEvents(){

    QString line, htmlLine,textBlock;
    if (locInformEvents.size() > 0){
        for (int i = 0; i <locInformEvents.size(); i++){
            QString timeStr;
            timeStr.setNum(locInformEvents[i].time);
            htmlLine = "<html><b>" + timeStr + "ms | "
                    + locInformEvents[i].errorType + ": </b></html>"
                    + locInformEvents[i].message + "<html><br></html>";

            textBlock.append(htmlLine);
        }
        ui->droneInform->setText(textBlock);
    }
    else {ui->droneInform->setText("No inform events occured/recorded");}

    if (locEmergencyEvents.size() > 0){
        for (int i = 0; i <locEmergencyEvents.size(); i++){
            QString timeStr;
            timeStr.setNum(locEmergencyEvents[i].time);
            htmlLine = "<html><b>" + timeStr + "ms | "
                    + locEmergencyEvents[i].errorType + ": </b></html>"
                    + locEmergencyEvents[i].message + "<html><br></html>";

            textBlock.append(htmlLine);
        }
        ui->droneEmergency->setText(textBlock);
    }
    else {ui->droneEmergency->setText("No emergency events occured/recorded");}

    if (locVerboseEvents.size() > 0){
        for (int i = 0; i <locVerboseEvents.size(); i++){
            QString timeStr;
            timeStr.setNum(locVerboseEvents[i].time);
            htmlLine = "<html><b>" + timeStr + "ms | "
                    + locVerboseEvents[i].errorType + ": </b></html>"
                    + locVerboseEvents[i].message + "<html><br></html>";

            textBlock.append(htmlLine);
        }
        ui->droneEvents->setText(textBlock);
    }
    else {ui->droneEvents->setText("No verbose events occured/recorded");}
}

void TripData::displayDroneStats(){
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

void TripData::on_HomeBtn_clicked()
{
    parentWidget()->show();
    close();
}

void TripData::on_DisplayModel_clicked()
{
    DataModel dataModel;
    dataModel.exec();
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

void TripData::help()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("../ArgouseBaseSoftware/appdata/docs/html/index.html"));
}



