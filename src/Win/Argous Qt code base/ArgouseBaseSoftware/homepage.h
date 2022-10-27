/****************************************************
 * Description: Header file for plotter.cpp
 * Author/s: Monique Kuhn
****************************************************/

#ifndef HOMEPAGE_H
#define HOMEPAGE_H

/***************************************************/

#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include <QVector>
#include <QFileDialog>
#include <QList>
#include <QtGui>
#include <QCloseEvent>
#include <QScrollArea>
#include <QDebug>
#include <iostream>
#include <QShortcut>

#include "ui_homepage.h"

/***************************************************/

class QAction;
class QMenu;
class QTextEdit;

namespace Ui {
class HomePage;
}

class HomePage : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage();

    typedef struct {
        int time;
        QString message;
        QString errorType;
    } DroneEvent;

    typedef struct {
        int time;
        float y;
    } DroneDataPoint;

    typedef struct {
        QString seriesName;
        QVector<DroneDataPoint> data;
    } DroneSeriesData;

    QVector<QString> importedData;          // Data from PLY File
    QVector<DroneEvent> informEvents;          // All Verbose and Inform events
    QVector<DroneEvent> verboseEvents;          // All Verbose and Inform events
    QVector<DroneEvent> emergencyEvents;    // Warning, Fatal, and Error events

    DroneSeriesData altitude;
    DroneSeriesData temperature;
    DroneSeriesData throttle;

    QAction *helpAct;
    QApplication *appParent;


private slots:
    void on_ViewTripDataBtn_clicked();
    void on_ImportBtn_clicked();
    void on_BrowseBtn_clicked();
    void on_ViewDroneDetsBtn_clicked();
    void on_RecentFilesLst_itemClicked(QListWidgetItem *_item);
    void on_ExportBtn_clicked();
    void on_HelpBtn_clicked();
    void on_ResetBtn_clicked();
    void on_FileBtn_clicked();
    void on_ClearBtn_clicked();

    void exportPLY();

    void about();
    void help();

    void darkMode();
    void lightMode();

private:
    Ui::HomePage *ui;

    void on_droneDetailClose_clicked();
    void on_droneDetailSave_clicked();
    void on_droneDetailReset_clicked();

    void usabilityTestSim();
    void writeMapToFile(QString _dest, QMap<QString, QString> *_map);
    void saveDroneDetails();
    void readDroneStats(); // Read in the drone stats from the appdata dir

    void readRecentFilesLog();
    void addRecentFile(QString _recentFile);
    void clearRecentFiles();
    void saveFile(const QString &_fileName);
    void setCurrentFile(const QString &_fileName);
    void updateRecentFileActions(const QString &_fullFileName);

    void stashTempPLY();

    void createActions();
    void createMenus();

    void closeEvent (QCloseEvent *_event);

    QWidget *Parent;

    QSettings settings;

    QTextEdit *textEdit;

    QString configFileLocation;
    QString strippedName(const QString &_fullFileName);
    QString curFile;
    QStringList files;

    QDialog *droneDetails = new QDialog;

    QMap<QString, QString> droneConfig; // The drone config as a key-value map
    QMap<QString, QString> droneDetailsMap; // The drone details as a key-value map
    QMap<QString, QString> defaultDroneDetailsMap; // The drone details as a key-value map
    QMap<QString, QString> runningStats; // The drone runningStats as a key-value map
    QMap<QString, QString> tripStats; // The drone tripStats as a key-value map

    QPushButton *detailsCloseButton ;
    QPushButton *detailSaveButton;
    QPushButton *detailResetButton;

    QMenu *fileMenu;
    QMenu *recentFilesMenu;
    QMenu *helpMenu;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;

    QAction *exportAct;
    QAction *exitAct;
    QAction *aboutAct;

    QAction *darkModeAct;
    QAction *lightModeAct;

    QAction *aboutQtAct;
    QAction *separatorAct;
    QAction *TestRecentFile;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];

    QVector<QString> RecentFilesLst;
    QQueue<QString> recentFilesQueue;

    int tripDuration = 0;
    int numberDataPoints = 0;
    int sampleRate = 0;
    int numberEvents;
    int numberCriticalEvents = 0;
    int flightDuration;
    int importDateTime;
    float averageTemp = 0;

    bool importFailed = true; // Set to true if any data import is not successful
    bool detailsOpened = false;

    bool tripEStopTriggered = false; // Flag to indicate that an E-Stop was hit
    bool tripOpOverrideTriggered = false; // Flag to indicate that an Operator Override occurrred

    bool importPLY(QString _droneCSVFile);  // Reads the CSV file from somewhere. Returns true if successful.
    bool importConf(QString _droneConfFile, QMap<QString, QString> *_dest); // Reads the config file as7.config. Returns true if successful.
    bool importLog(QString _droneLogFile);  // Reads in the log as events. Returns true if successful.

    DroneSeriesData extractThrottleValues(QVector<DroneEvent> _droneLogData);
    DroneSeriesData readColumnFromCSV(QString _dataFile, QString _colName);
    DroneSeriesData readTempValues(QString _dataFile);
    DroneSeriesData readAltValues(QString _dataFile);
};

#endif // HOMEPAGE_H
