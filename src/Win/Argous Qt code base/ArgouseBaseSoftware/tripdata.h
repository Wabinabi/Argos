/****************************************************
 * Description: Header file for plotter.cpp
 * Author/s: Monique Kuhn
****************************************************/

#ifndef TRIPDATA_H
#define TRIPDATA_H

#include <QDialog>

/***************************************************/

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCore/QDateTime>
#include <QtCharts/QDateTimeAxis>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCharts/QValueAxis>
#include <QMainWindow>
#include <QtCharts/QChartGlobal>
#include <QtCharts/QScatterSeries>
#include <QStackedLayout>
#include <QtWidgets/QRubberBand>

#include <homepage.h>

/***************************************************/

QT_USE_NAMESPACE

namespace Ui {
class TripData;
}

class TripData : public QDialog
{
    Q_OBJECT

public:
    TripData(QWidget *parent = nullptr,
             QVector<HomePage::DroneEvent> *_emergencyEvents = nullptr,
             QVector<HomePage::DroneEvent> *_verboseEvents = nullptr,
             QVector<HomePage::DroneEvent> *_informEvents = nullptr,
             HomePage::DroneSeriesData *_altitude = nullptr,
             HomePage::DroneSeriesData *_temperature = nullptr,
             HomePage::DroneSeriesData *_throttle = nullptr);
    ~TripData();

    //void testPublicFunction();

private slots:
    void keyPressEvent(QKeyEvent *event);

    void on_HomeBtn_clicked();
    void on_DisplayModel_clicked();
    void on_ThrottleBtn_clicked();
    void on_TempBtn_clicked();
    void on_AltitudeBtn_clicked();

    void drawStackedTrends();
    void highlightTrendButton(int _index);
    void drawXYSeries(int _stackIndex, HomePage::DroneSeriesData _droneData, QChart *_selChart, QChartView *_selChartView);
    void displayDroneEvents();
    void displayDroneStats();
    void help();

private:
    QVector<HomePage::DroneEvent> locEmergencyEvents;
    QVector<HomePage::DroneEvent> locVerboseEvents;
    QVector<HomePage::DroneEvent> locInformEvents;

    HomePage::DroneSeriesData locAltitude;
    HomePage::DroneSeriesData locTemperature;
    HomePage::DroneSeriesData locThrottle;

    Ui::TripData *ui;
    QScatterSeries *scatter;
    QScatterSeries *scatter2;
    QScatterSeries *scatter3;

    QChart *currentChart = new QChart();
    QChart *eventsChart = new QChart();
    QChart *tempChart = new QChart();
    QChart *throttleChart = new QChart();
    QChart *altitudeChart = new QChart();

    QChartView *eventsChartView = new QChartView(eventsChart);
    QChartView *tempChartView = new QChartView(tempChart);
    QChartView *throttleChartView = new QChartView(throttleChart);
    QChartView *altitudeChartView = new QChartView(altitudeChart);

    int eventsIndex;
    int tempIndex;
    int throttleIndex;
    int altitudeIndex;

    bool isTouching;
};

#endif // TRIPDATA_H
