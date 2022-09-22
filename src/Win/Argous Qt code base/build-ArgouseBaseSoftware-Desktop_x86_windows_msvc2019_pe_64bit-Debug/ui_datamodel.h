/********************************************************************************
** Form generated from reading UI file 'datamodel.ui'
**
** Created by: Qt User Interface Compiler version 6.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATAMODEL_H
#define UI_DATAMODEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_DataModel
{
public:

    void setupUi(QDialog *DataModel)
    {
        if (DataModel->objectName().isEmpty())
            DataModel->setObjectName(QString::fromUtf8("DataModel"));
        DataModel->resize(400, 300);

        retranslateUi(DataModel);

        QMetaObject::connectSlotsByName(DataModel);
    } // setupUi

    void retranslateUi(QDialog *DataModel)
    {
        DataModel->setWindowTitle(QCoreApplication::translate("DataModel", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DataModel: public Ui_DataModel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATAMODEL_H
