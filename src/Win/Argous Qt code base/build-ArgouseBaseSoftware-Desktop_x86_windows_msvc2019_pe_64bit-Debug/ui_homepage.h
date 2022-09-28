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
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HomePage
{
public:
    QAction *actionImport;
    QWidget *centralwidget;
    QFrame *ImportData;
    QPlainTextEdit *FileLocation;
    QPushButton *BrowseBtn;
    QPushButton *ImportBtn;
    QPushButton *pushButton;
    QLabel *label;
    QToolButton *toolButton;
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
    QFrame *frame_3;
    QLabel *label_5;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QMenuBar *menubar;
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
        ImportData = new QFrame(centralwidget);
        ImportData->setObjectName(QString::fromUtf8("ImportData"));
        ImportData->setGeometry(QRect(130, 30, 401, 151));
        ImportData->setFrameShape(QFrame::StyledPanel);
        ImportData->setFrameShadow(QFrame::Raised);
        FileLocation = new QPlainTextEdit(ImportData);
        FileLocation->setObjectName(QString::fromUtf8("FileLocation"));
        FileLocation->setGeometry(QRect(10, 30, 291, 31));
        BrowseBtn = new QPushButton(ImportData);
        BrowseBtn->setObjectName(QString::fromUtf8("BrowseBtn"));
        BrowseBtn->setGeometry(QRect(310, 30, 81, 31));
        ImportBtn = new QPushButton(ImportData);
        ImportBtn->setObjectName(QString::fromUtf8("ImportBtn"));
        ImportBtn->setGeometry(QRect(120, 70, 181, 31));
        pushButton = new QPushButton(ImportData);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(10, 70, 101, 31));
        label = new QLabel(ImportData);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 0, 131, 16));
        toolButton = new QToolButton(ImportData);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        toolButton->setGeometry(QRect(280, 30, 21, 31));
        DroneVisual = new QFrame(centralwidget);
        DroneVisual->setObjectName(QString::fromUtf8("DroneVisual"));
        DroneVisual->setGeometry(QRect(540, 30, 241, 281));
        DroneVisual->setFrameShape(QFrame::StyledPanel);
        DroneVisual->setFrameShadow(QFrame::Raised);
        label_2 = new QLabel(DroneVisual);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 0, 131, 16));
        pushButton_4 = new QPushButton(DroneVisual);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(10, 240, 101, 31));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(540, 319, 241, 221));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label_3 = new QLabel(frame);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 10, 131, 16));
        scrollArea = new QScrollArea(frame);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(10, 30, 211, 181));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 209, 179));
        verticalScrollBar = new QScrollBar(scrollAreaWidgetContents);
        verticalScrollBar->setObjectName(QString::fromUtf8("verticalScrollBar"));
        verticalScrollBar->setGeometry(QRect(280, 0, 16, 181));
        verticalScrollBar->setOrientation(Qt::Vertical);
        scrollArea->setWidget(scrollAreaWidgetContents);
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(130, 190, 401, 351));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        label_4 = new QLabel(frame_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 10, 131, 16));
        scrollArea_2 = new QScrollArea(frame_2);
        scrollArea_2->setObjectName(QString::fromUtf8("scrollArea_2"));
        scrollArea_2->setGeometry(QRect(10, 40, 381, 301));
        scrollArea_2->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 379, 299));
        verticalScrollBar_2 = new QScrollBar(scrollAreaWidgetContents_2);
        verticalScrollBar_2->setObjectName(QString::fromUtf8("verticalScrollBar_2"));
        verticalScrollBar_2->setGeometry(QRect(390, 0, 16, 291));
        verticalScrollBar_2->setOrientation(Qt::Vertical);
        scrollArea_2->setWidget(scrollAreaWidgetContents_2);
        frame_3 = new QFrame(centralwidget);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setGeometry(QRect(-1, -1, 111, 561));
        frame_3->setAutoFillBackground(false);
        frame_3->setStyleSheet(QString::fromUtf8("background-color: rgb(69, 142, 149);"));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        label_5 = new QLabel(frame_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(0, 10, 111, 91));
        label_5->setPixmap(QPixmap(QString::fromUtf8("../../Graphic Assets/15ppi/logoArtboard 5_1.png")));
        label_5->setAlignment(Qt::AlignCenter);
        pushButton_2 = new QPushButton(frame_3);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(0, 510, 111, 31));
        pushButton_2->setStyleSheet(QString::fromUtf8("Text-align:left;\n"
"padding: 10px;"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../Graphic Assets/icons/15ppi/logoArtboard 6.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2->setIcon(icon);
        pushButton_2->setIconSize(QSize(20, 16));
        pushButton_3 = new QPushButton(frame_3);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(0, 480, 111, 31));
        pushButton_3->setStyleSheet(QString::fromUtf8("Text-align:left;\n"
"padding: 10px;"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("../../Graphic Assets/icons/15ppi/iconArtboard 8.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_3->setIcon(icon1);
        HomePage->setCentralWidget(centralwidget);
        menubar = new QMenuBar(HomePage);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        HomePage->setMenuBar(menubar);
        statusbar = new QStatusBar(HomePage);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        HomePage->setStatusBar(statusbar);

        retranslateUi(HomePage);

        QMetaObject::connectSlotsByName(HomePage);
    } // setupUi

    void retranslateUi(QMainWindow *HomePage)
    {
        HomePage->setWindowTitle(QCoreApplication::translate("HomePage", "MainWindow", nullptr));
        actionImport->setText(QCoreApplication::translate("HomePage", "Import", nullptr));
        BrowseBtn->setText(QCoreApplication::translate("HomePage", "Browse", nullptr));
        ImportBtn->setText(QCoreApplication::translate("HomePage", "Import", nullptr));
        pushButton->setText(QCoreApplication::translate("HomePage", "Trip Data", nullptr));
        label->setText(QCoreApplication::translate("HomePage", "Import Data Widget", nullptr));
        toolButton->setText(QCoreApplication::translate("HomePage", "...", nullptr));
        label_2->setText(QCoreApplication::translate("HomePage", "Drone specs", nullptr));
        pushButton_4->setText(QCoreApplication::translate("HomePage", "Drone Details", nullptr));
        label_3->setText(QCoreApplication::translate("HomePage", "Drone History", nullptr));
        label_4->setText(QCoreApplication::translate("HomePage", "Recent Files", nullptr));
        label_5->setText(QString());
        pushButton_2->setText(QCoreApplication::translate("HomePage", "  Settings", nullptr));
        pushButton_3->setText(QCoreApplication::translate("HomePage", "  Reset", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HomePage: public Ui_HomePage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HOMEPAGE_H
