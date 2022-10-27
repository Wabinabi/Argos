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

    /*Populate window with data*/
    drawStackedTrends();
    displayDroneStats();
    displayDroneEvents();
}


TripData::~TripData()
{
    delete ui;
}

/***************************************************/

/*Draw each trend into the display stack, disable any trends that don't have data*/
void TripData::drawStackedTrends(){
    /*Indexes for the stacked widget*/
    tempIndex = 0;
    throttleIndex = 1;
    altitudeIndex = 2;
    int defaultIndex = 0;

    /*Draw each trend into the stacked widget*/
    drawXYSeries(tempIndex, locTemperature, tempChart, tempChartView);
    drawXYSeries(throttleIndex, locThrottle, throttleChart, throttleChartView);
    drawXYSeries(altitudeIndex, locAltitude, altitudeChart, altitudeChartView);

    /*Disable trend button if its data is not available*/
    /*Throttle*/
    if (locThrottle.data.size() == 0) {
        ui->throttleBtn->setEnabled(false);
        ui->throttleBtn->setText("Throttle not Enabled");
    } else {
        ui->throttleBtn->setEnabled(true);
        ui->throttleBtn->setText("Display Throttle Trend");
    }

    /*Altitude*/
    if (locThrottle.data.size() == 0) {
        ui->altitudeBtn->setEnabled(false);
        ui->altitudeBtn->setText("Altitude not Enabled");
    } else {
        ui->altitudeBtn->setEnabled(true);
        ui->altitudeBtn->setText("Display Altitude Trend");
    }

    /*Temperature*/
    if (locThrottle.data.size() == 0) {
        ui->tempBtn->setEnabled(false);
        ui->tempBtn->setText("Temperature not Enabled");
    } else {
        ui->tempBtn->setEnabled(true);
        ui->tempBtn->setText("Display Temperature Trend");
    }

    /*Set the default trend*/
    ui->stackedWidget->setCurrentIndex(defaultIndex);
    highlightTrendButton(defaultIndex);
}

/*Update which button is highlighted based on the index passed through*/
void TripData::highlightTrendButton(int _index){
    switch (_index)  {
        case 0:
            ui->tempBtn->setStyleSheet("background-color: #FF752B");
            ui->throttleBtn->setStyleSheet("background-color: ");
            ui->altitudeBtn->setStyleSheet("background-color: ");
            break;

        case 1:
            ui->tempBtn->setStyleSheet("background-color: ");
            ui->throttleBtn->setStyleSheet("background-color: #FF752B");
            ui->altitudeBtn->setStyleSheet("background-color: ");
            break;

        case 2:
        ui->tempBtn->setStyleSheet("background-color: ");
        ui->throttleBtn->setStyleSheet("background-color: ");
        ui->altitudeBtn->setStyleSheet("background-color: #FF752B");
        break;

        default:
            ui->throttleBtn->setStyleSheet("background-color: ");
            ui->tempBtn->setStyleSheet("background-color: ");
            ui->altitudeBtn->setStyleSheet("background-color: ");
    }
}

/*This function draws the XY series based on the index and data passed through*/
void TripData::drawXYSeries(int _stackIndex, HomePage::DroneSeriesData _droneData, QChart *_selChart, QChartView *_selChartView){
    /*Convert data - Append the data to an XY series*/
    QLineSeries *_XYSeries = new QLineSeries();
    for (int i = 0; i < _droneData.data.size(); ++i) {
        _XYSeries->append(_droneData.data[i].time/1000, _droneData.data[i].y);
    }

    /*Setup the chart with X/Y axis details*/
    _selChart->addSeries(_XYSeries);
    _selChart->legend()->hide();

    //X Axis
    QValueAxis *axisX = new QValueAxis;

    axisX->setTitleText("Time (s)");
    _selChart->addAxis(axisX, Qt::AlignBottom);
    _XYSeries->attachAxis(axisX);

    //Y Axis
    QValueAxis *axisY = new QValueAxis;

    if (_stackIndex == 0){axisY->setTitleText("Temperature (deg c)");}
    if (_stackIndex == 1 ){axisY->setTitleText("Throttle (x/2056)");}
    if (_stackIndex == 2){axisY->setTitleText("Height above ground (cm)");}

    _selChart->addAxis(axisY, Qt::AlignLeft);
    _XYSeries->attachAxis(axisY);
    _selChartView->setRenderHint(QPainter::Antialiasing);

    /*Insert the XY trend into the widget*/
    ui->stackedWidget->insertWidget(_stackIndex, _selChartView);
}

/*Display the drone events in three seperate text boxes*/
void TripData::displayDroneEvents(){
    QString line, htmlLine,textBlock;
    /*Inform Events*/
    if (locInformEvents.size() > 0){
        for (int i = 0; i <locInformEvents.size(); i++){
            QString timeStr;
            timeStr.setNum(locInformEvents[i].time);
            htmlLine = "<html><b>" + timeStr + "ms | "
                    + locInformEvents[i].errorType + ": </b></html>"
                    + locInformEvents[i].message + "<html><br></html>";

            textBlock.append(htmlLine);
        }
        ui->droneInformTxt->setText(textBlock);
    }
    else {ui->droneInformTxt->setText("No inform events occured/recorded");}

    /*Emergency Events*/
    if (locEmergencyEvents.size() > 0){
        for (int i = 0; i <locEmergencyEvents.size(); i++){
            QString timeStr;
            timeStr.setNum(locEmergencyEvents[i].time);
            htmlLine = "<html><b>" + timeStr + "ms | "
                    + locEmergencyEvents[i].errorType + ": </b></html>"
                    + locEmergencyEvents[i].message + "<html><br></html>";

            textBlock.append(htmlLine);
        }
        ui->droneEmergencyTxt->setText(textBlock);
    }
    else {ui->droneEmergencyTxt->setText("No emergency events occured/recorded");}

    /*Verbose Events*/
    if (locVerboseEvents.size() > 0){
        for (int i = 0; i <locVerboseEvents.size(); i++){
            QString timeStr;
            timeStr.setNum(locVerboseEvents[i].time);
            htmlLine = "<html><b>" + timeStr + "ms | "
                    + locVerboseEvents[i].errorType + ": </b></html>"
                    + locVerboseEvents[i].message + "<html><br></html>";

            textBlock.append(htmlLine);
        }
        ui->droneVerboseTxt->setText(textBlock);
    }
    else {ui->droneVerboseTxt->setText("No verbose events occured/recorded");}
}

/*Display drone stats in text box*/
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

        ui->droneStatsTxt->setText(textBlock);
    }
    file.close();
}

/*Generate and display the 3D visualisation*/
void TripData::on_displayModelBtn_clicked()
{
    DataModel dataModel;
    dataModel.exec();
}

/*Trend buttons trigger the correct stacked widget to display*/
void TripData::on_throttleBtn_clicked()
{
    highlightTrendButton(throttleIndex);
    ui->stackedWidget->setCurrentIndex(throttleIndex);
    currentChart = throttleChart;
}

void TripData::on_tempBtn_clicked()
    {
        highlightTrendButton(tempIndex);
        ui->stackedWidget->setCurrentIndex(tempIndex);
        currentChart = tempChart;
    }

void TripData::on_altitudeBtn_clicked()
{
    highlightTrendButton(altitudeIndex);
    ui->stackedWidget->setCurrentIndex(altitudeIndex);
    currentChart = altitudeChart;
}

/*Generate response to any key presses - this is used in the 3D visualisation (data model)*/
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

/*Load help*/
void TripData::help()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("../ArgouseBaseSoftware/appdata/docs/html/index.html"));
}













