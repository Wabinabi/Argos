#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QMainWindow>

#include "tripdata.h"

namespace Ui {
class HomePage;
}

class HomePage : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage();

private slots:
    void on_pushButton_clicked();

private:
    Ui::HomePage *ui;
};

#endif // HOMEPAGE_H
