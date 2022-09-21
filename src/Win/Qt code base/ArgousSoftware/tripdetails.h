#ifndef TRIPDETAILS_H
#define TRIPDETAILS_H

#include <QDialog>

namespace Ui {
class TripDetails;
}

class TripDetails : public QDialog
{
    Q_OBJECT

public:
    explicit TripDetails(QWidget *parent = nullptr);
    ~TripDetails();

private:
    Ui::TripDetails *ui;
};

#endif // TRIPDETAILS_H
