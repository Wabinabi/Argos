/********************************************************************************
** Form generated from reading UI file 'dronespecs.ui'
**
** Created by: Qt User Interface Compiler version 6.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DRONESPECS_H
#define UI_DRONESPECS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_DroneSpecs
{
public:

    void setupUi(QDialog *DroneSpecs)
    {
        if (DroneSpecs->objectName().isEmpty())
            DroneSpecs->setObjectName(QString::fromUtf8("DroneSpecs"));
        DroneSpecs->resize(400, 300);

        retranslateUi(DroneSpecs);

        QMetaObject::connectSlotsByName(DroneSpecs);
    } // setupUi

    void retranslateUi(QDialog *DroneSpecs)
    {
        DroneSpecs->setWindowTitle(QCoreApplication::translate("DroneSpecs", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DroneSpecs: public Ui_DroneSpecs {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DRONESPECS_H
