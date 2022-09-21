#ifndef ARGOUS_H
#define ARGOUS_H

#include <QMainWindow>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>

#include "dronespecs.h"
#include "tripdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Argous; }
QT_END_NAMESPACE

class Argous : public QMainWindow
{
    Q_OBJECT

public:
    Argous(QWidget *parent = nullptr);
    ~Argous();

private slots:

    void on_TempTripBtn_clicked();

    void on_DroneSpecsBtn_clicked();

    void on_OpenTxtFileBtn_clicked();


private:
    Ui::Argous *ui;
    TripData *tripDataPage;
};
#endif // ARGOUS_H
