/********************************************************************************
** Form generated from reading UI file 'homepage.ui'
**
** Created by: Qt User Interface Compiler version 6.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HOMEPAGE_H
#define UI_HOMEPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HomePage
{
public:
    QAction *actionImport;
    QWidget *centralwidget;
    QLabel *label;
    QFrame *ImportData;
    QPlainTextEdit *plainTextEdit;
    QPlainTextEdit *plainTextEdit_2;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton;
    QFrame *DroneVisual;
    QLabel *label_2;
    QPushButton *pushButton_4;
    QFrame *frame;
    QLabel *label_3;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QScrollBar *verticalScrollBar;
    QFrame *frame_2;
    QLabel *label_4;
    QScrollArea *scrollArea_2;
    QWidget *scrollAreaWidgetContents_2;
    QScrollBar *verticalScrollBar_2;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *HomePage)
    {
        if (HomePage->objectName().isEmpty())
            HomePage->setObjectName(QString::fromUtf8("HomePage"));
        HomePage->resize(800, 600);
        actionImport = new QAction(HomePage);
        actionImport->setObjectName(QString::fromUtf8("actionImport"));
        centralwidget = new QWidget(HomePage);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 131, 16));
        ImportData = new QFrame(centralwidget);
        ImportData->setObjectName(QString::fromUtf8("ImportData"));
        ImportData->setGeometry(QRect(10, 30, 441, 151));
        ImportData->setFrameShape(QFrame::StyledPanel);
        ImportData->setFrameShadow(QFrame::Raised);
        plainTextEdit = new QPlainTextEdit(ImportData);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(10, 30, 291, 31));
        plainTextEdit_2 = new QPlainTextEdit(ImportData);
        plainTextEdit_2->setObjectName(QString::fromUtf8("plainTextEdit_2"));
        plainTextEdit_2->setGeometry(QRect(10, 70, 291, 31));
        pushButton_2 = new QPushButton(ImportData);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(310, 30, 101, 31));
        pushButton_3 = new QPushButton(ImportData);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(310, 70, 101, 31));
        pushButton = new QPushButton(ImportData);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(310, 110, 101, 31));
        DroneVisual = new QFrame(centralwidget);
        DroneVisual->setObjectName(QString::fromUtf8("DroneVisual"));
        DroneVisual->setGeometry(QRect(460, 30, 321, 281));
        DroneVisual->setFrameShape(QFrame::StyledPanel);
        DroneVisual->setFrameShadow(QFrame::Raised);
        label_2 = new QLabel(DroneVisual);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 0, 131, 16));
        pushButton_4 = new QPushButton(DroneVisual);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(210, 240, 101, 31));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(460, 319, 321, 221));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label_3 = new QLabel(frame);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 10, 131, 16));
        scrollArea = new QScrollArea(frame);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(9, 29, 301, 181));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 299, 179));
        verticalScrollBar = new QScrollBar(scrollAreaWidgetContents);
        verticalScrollBar->setObjectName(QString::fromUtf8("verticalScrollBar"));
        verticalScrollBar->setGeometry(QRect(280, 0, 16, 181));
        verticalScrollBar->setOrientation(Qt::Vertical);
        scrollArea->setWidget(scrollAreaWidgetContents);
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(10, 190, 441, 351));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        label_4 = new QLabel(frame_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 10, 131, 16));
        scrollArea_2 = new QScrollArea(frame_2);
        scrollArea_2->setObjectName(QString::fromUtf8("scrollArea_2"));
        scrollArea_2->setGeometry(QRect(10, 40, 411, 291));
        scrollArea_2->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 409, 289));
        verticalScrollBar_2 = new QScrollBar(scrollAreaWidgetContents_2);
        verticalScrollBar_2->setObjectName(QString::fromUtf8("verticalScrollBar_2"));
        verticalScrollBar_2->setGeometry(QRect(390, 0, 16, 291));
        verticalScrollBar_2->setOrientation(Qt::Vertical);
        scrollArea_2->setWidget(scrollAreaWidgetContents_2);
        HomePage->setCentralWidget(centralwidget);
        ImportData->raise();
        label->raise();
        DroneVisual->raise();
        frame->raise();
        frame_2->raise();
        menubar = new QMenuBar(HomePage);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        HomePage->setMenuBar(menubar);
        statusbar = new QStatusBar(HomePage);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        HomePage->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionImport);

        retranslateUi(HomePage);

        QMetaObject::connectSlotsByName(HomePage);
    } // setupUi

    void retranslateUi(QMainWindow *HomePage)
    {
        HomePage->setWindowTitle(QCoreApplication::translate("HomePage", "MainWindow", nullptr));
        actionImport->setText(QCoreApplication::translate("HomePage", "Import", nullptr));
        label->setText(QCoreApplication::translate("HomePage", "Import Data Widget", nullptr));
        pushButton_2->setText(QCoreApplication::translate("HomePage", "Browse", nullptr));
        pushButton_3->setText(QCoreApplication::translate("HomePage", "Import", nullptr));
        pushButton->setText(QCoreApplication::translate("HomePage", "Trip Data", nullptr));
        label_2->setText(QCoreApplication::translate("HomePage", "Drone Artwork", nullptr));
        pushButton_4->setText(QCoreApplication::translate("HomePage", "Drone Details", nullptr));
        label_3->setText(QCoreApplication::translate("HomePage", "Drone History", nullptr));
        label_4->setText(QCoreApplication::translate("HomePage", "Recent Files", nullptr));
        menuFile->setTitle(QCoreApplication::translate("HomePage", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HomePage: public Ui_HomePage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HOMEPAGE_H
