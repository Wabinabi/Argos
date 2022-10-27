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
             QVector<HomePage::DroneEvent> *emergencyEvents = nullptr,
             QVector<HomePage::DroneEvent> *verboseEvents = nullptr,
             QVector<HomePage::DroneEvent> *informEvents = nullptr,
             HomePage::DroneSeriesData *altitude = nullptr,
             HomePage::DroneSeriesData *temperature = nullptr,
             HomePage::DroneSeriesData *throttle = nullptr);
    ~TripData();

    void testPublicFunction();

protected:


private slots:
    //bool viewportEvent(QEvent *event);
    //void mousePressEvent(QMouseEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);
    //void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    //UI buttons
    void on_HomeBtn_clicked();
    void on_DisplayModel_clicked();

    //Plotting 2D data
    //QLineSeries reshapeVector(QVector<QString> XYData);
    //QLineSeries *XYSeries
    void drawXYSeries(int stackIndex, HomePage::DroneSeriesData droneData, QChart *selChart, QChartView *selChartView);
    void displayDroneEvents();
    //void drawXYSeries(QVector<QString> XYData);

    //UI buttons, these set the index associated with the page sought after by the user
    //void on_EventsBtn_clicked();
    void on_ThrottleBtn_clicked();
    void on_TempBtn_clicked();
    void on_AltitudeBtn_clicked();
    void displayDroneStats();
    void help();

    void drawStackedTrends();
    void highlightTrendButton(int _index);

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


    //Chart Views for 2D plot & stacked layout

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
