#ifndef TRIPDATA_H
#define TRIPDATA_H

#include <QDialog>

//ALL THE FREAKING INCLUDES FOR A GRAPH
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

QT_USE_NAMESPACE

namespace Ui {
class TripData;
}

class TripData : public QDialog
{
    Q_OBJECT

public:
    explicit TripData(QWidget *parent = nullptr);
    ~TripData();
    //void closeEvent(QCloseEvent *event) override;

private slots:

    //UI buttons
    void on_HomeBtn_clicked();
    void on_DisplayModel_clicked();

    //Plotting 2D data
    QLineSeries reshapeVector(QVector<QString> XYData);
    void drawXYSeries(QLineSeries XYSeries);

private:
    Ui::TripData *ui;

};

#endif // TRIPDATA_H
