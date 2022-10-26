#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "plotter.h"

#include <QDialog>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtGui/QScreen>
#include <QtGui/QFontDatabase>
#include <QCloseEvent>

namespace Ui {class DataModel;}

class DataModel : public QDialog
{
    Q_OBJECT

public:
    explicit DataModel(QWidget *parent = nullptr);
    ~DataModel();

private:
    Ui::DataModel *ui;
    void generate3DWidget();
    void showEvent( QShowEvent* _event );

    Plotter* modifier;
};

#endif // DATAMODEL_H
