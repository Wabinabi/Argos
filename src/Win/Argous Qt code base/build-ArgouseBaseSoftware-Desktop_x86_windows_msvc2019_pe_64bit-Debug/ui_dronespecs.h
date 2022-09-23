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
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_DroneSpecs
{
public:
    QLabel *label;
    QFrame *DroneDetails;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QFrame *frame;
    QLabel *label_9;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *DroneSpecs)
    {
        if (DroneSpecs->objectName().isEmpty())
            DroneSpecs->setObjectName(QString::fromUtf8("DroneSpecs"));
        DroneSpecs->resize(407, 249);
        label = new QLabel(DroneSpecs);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 71, 16));
        DroneDetails = new QFrame(DroneSpecs);
        DroneDetails->setObjectName(QString::fromUtf8("DroneDetails"));
        DroneDetails->setGeometry(QRect(10, 30, 161, 181));
        DroneDetails->setFrameShape(QFrame::StyledPanel);
        DroneDetails->setFrameShadow(QFrame::Raised);
        label_2 = new QLabel(DroneDetails);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 10, 71, 16));
        label_3 = new QLabel(DroneDetails);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(0, 30, 91, 16));
        label_4 = new QLabel(DroneDetails);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(0, 50, 91, 16));
        label_5 = new QLabel(DroneDetails);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(0, 70, 101, 16));
        label_6 = new QLabel(DroneDetails);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(0, 90, 101, 16));
        label_7 = new QLabel(DroneDetails);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(0, 110, 91, 16));
        label_8 = new QLabel(DroneDetails);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(0, 130, 91, 16));
        frame = new QFrame(DroneSpecs);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(180, 30, 211, 181));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label_9 = new QLabel(frame);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 10, 71, 16));
        pushButton = new QPushButton(DroneSpecs);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(310, 220, 75, 24));
        pushButton_2 = new QPushButton(DroneSpecs);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(230, 220, 75, 24));

        retranslateUi(DroneSpecs);

        QMetaObject::connectSlotsByName(DroneSpecs);
    } // setupUi

    void retranslateUi(QDialog *DroneSpecs)
    {
        DroneSpecs->setWindowTitle(QCoreApplication::translate("DroneSpecs", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("DroneSpecs", "Drone Details", nullptr));
        label_2->setText(QCoreApplication::translate("DroneSpecs", "Model", nullptr));
        label_3->setText(QCoreApplication::translate("DroneSpecs", "Firmware Version", nullptr));
        label_4->setText(QCoreApplication::translate("DroneSpecs", "Software Versions", nullptr));
        label_5->setText(QCoreApplication::translate("DroneSpecs", "Operating License", nullptr));
        label_6->setText(QCoreApplication::translate("DroneSpecs", "Last Maintenance", nullptr));
        label_7->setText(QCoreApplication::translate("DroneSpecs", "No. of Flights", nullptr));
        label_8->setText(QCoreApplication::translate("DroneSpecs", "Total Flight Time", nullptr));
        label_9->setText(QCoreApplication::translate("DroneSpecs", "Drone Parts", nullptr));
        pushButton->setText(QCoreApplication::translate("DroneSpecs", "Close", nullptr));
        pushButton_2->setText(QCoreApplication::translate("DroneSpecs", "Edit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DroneSpecs: public Ui_DroneSpecs {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DRONESPECS_H
