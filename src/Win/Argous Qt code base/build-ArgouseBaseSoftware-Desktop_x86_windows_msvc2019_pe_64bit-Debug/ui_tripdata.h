/********************************************************************************
** Form generated from reading UI file 'tripdata.ui'
**
** Created by: Qt User Interface Compiler version 6.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRIPDATA_H
#define UI_TRIPDATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_TripData
{
public:
    QPushButton *HomeBtn;
    QPushButton *DisplayModel;

    void setupUi(QDialog *TripData)
    {
        if (TripData->objectName().isEmpty())
            TripData->setObjectName(QString::fromUtf8("TripData"));
        TripData->resize(400, 300);
        HomeBtn = new QPushButton(TripData);
        HomeBtn->setObjectName(QString::fromUtf8("HomeBtn"));
        HomeBtn->setGeometry(QRect(10, 10, 141, 31));
        DisplayModel = new QPushButton(TripData);
        DisplayModel->setObjectName(QString::fromUtf8("DisplayModel"));
        DisplayModel->setGeometry(QRect(10, 50, 141, 31));

        retranslateUi(TripData);

        QMetaObject::connectSlotsByName(TripData);
    } // setupUi

    void retranslateUi(QDialog *TripData)
    {
        TripData->setWindowTitle(QCoreApplication::translate("TripData", "Dialog", nullptr));
        HomeBtn->setText(QCoreApplication::translate("TripData", "Home", nullptr));
        DisplayModel->setText(QCoreApplication::translate("TripData", "Display Model", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TripData: public Ui_TripData {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRIPDATA_H
