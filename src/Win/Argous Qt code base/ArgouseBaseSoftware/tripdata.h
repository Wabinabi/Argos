#ifndef TRIPDATA_H
#define TRIPDATA_H

#include <QDialog>

namespace Ui {
class TripData;
}

class TripData : public QDialog
{
    Q_OBJECT

public:
    explicit TripData(QWidget *parent = nullptr);
    ~TripData();
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_HomeBtn_clicked();

    void on_DisplayModel_clicked();

private:
    Ui::TripData *ui;

};

#endif // TRIPDATA_H
