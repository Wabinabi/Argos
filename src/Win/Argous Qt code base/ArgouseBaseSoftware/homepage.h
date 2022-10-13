#ifndef HOMEPAGE_H
#define HOMEPAGE_H

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

//#include "tripdata.h"
#include "datatranslator.h"
#include "ui_homepage.h"

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

//TripData* tripData = nullptr;

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

//    // Import/process data
//    //  We're assuming that the user imports one trip at a time
//    //  and therefore the following repo-style variables will relate
//    //  to only one trip
    QVector<QString> importedData;          // Data from PLY File
    QVector<DroneEvent> informEvents;          // All Verbose and Inform events
    // Processed data
    QVector<DroneEvent> verboseEvents;          // All Verbose and Inform events
    QVector<DroneEvent> emergencyEvents;    // Warning, Fatal, and Error events

    DroneSeriesData altitude;
    DroneSeriesData temperature;
    DroneSeriesData throttle;


private slots:

    // Buttons
    void on_pushButton_clicked();
    void on_ImportBtn_clicked();
    void on_Browse_clicked();
    void on_pushButton_4_clicked();

    //Recent Files
    //void newFile();
    //void open();
    //void save();
    void exportPLY();
    //void openRecentFile();
    void about();
    void help();


    void on_recentFiles_itemClicked(QListWidgetItem *item);

private:
    Ui::HomePage *ui;
    void UsabilityTestSim();

    QMap<QString, QString> droneConfig; // The drone config as a key-value map
    QMap<QString, QString> droneDetailsMap; // The drone details as a key-value map
    QMap<QString, QString> defaultDroneDetailsMap; // The drone details as a key-value map

    QMap<QString, QString> runningStats; // The drone runningStats as a key-value map
    QMap<QString, QString> tripStats; // The drone tripStats as a key-value map


    // All of the following are set at import
    int tripDuration = 0;
    int numberDataPoints = 0;
    int sampleRate = 0;
    float averageTemp = 0;
    int numberEvents;
    int numberCriticalEvents = 0;

    void writeMapToFile(QString dest, QMap<QString, QString> *map);

    bool importFailed = true; // Set to true if any data import is not successful

    // Read coils for SCADA, essentially.
    bool tripEStopTriggered = false; // Flag to indicate that an E-Stop was hit
    bool tripOpOverrideTriggered = false; // Flag to indicate that an Operator Override occurrred

    // Statistics/Information
    int flightDuration; //
    int importDateTime;

    DroneSeriesData extractThrottleValues(QVector<DroneEvent> droneLogData);
    DroneSeriesData readColumnFromCSV(QString dataFile, QString colName);
    DroneSeriesData readTempValues(QString dataFile);
    DroneSeriesData readAltValues(QString dataFile);

    QString configFileLocation;

    void saveDroneDetails();

    void on_droneDetailClose_clicked();
    void on_droneDetailSave_clicked();
    void on_droneDetailReset_clicked();

    QPushButton *detailsCloseButton ;
    //B1-Edit detail buttons
    QPushButton *detailSaveButton;
    QPushButton *detailResetButton;

    QWidget *droneDetails = new QWidget;
    bool detailsOpened = false;


    void readDroneStats(); // Read in the drone stats from the appdata dir
    bool importPLY(QString droneCSVFile);  // Reads the CSV file from somewhere. Returns true if successful.
    bool importConf(QString droneConfFile, QMap<QString, QString> *dest); // Reads the config file as7.config. Returns true if successful.
    bool importLog(QString droneLogFile);  // Reads in the log as events. Returns true if successful.

    //Read logs
    void readRecentFilesLog();
    void addRecentFile(QString recentFile);
    QVector<QString> recentFiles;
    QQueue<QString> recentFilesQueue;

    //Create Temp Working files
    void stashTempPLY();

    //Close
    void closeEvent (QCloseEvent *event);

    //Recent Files
    void createActions();
    void createMenus();
    //void loadFile(const QString &fileName);
    void saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions(const QString &fullFileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;

    QStringList files;
    QSettings settings;
    QTextEdit *textEdit;
    QMenu *fileMenu;
    QMenu *recentFilesMenu;
    QMenu *helpMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    //QAction *saveAsAct;
    QAction *exportAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *helpAct;
    QAction *aboutQtAct;
    QAction *separatorAct;
    QAction *TestRecentFile;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];

};

#endif // HOMEPAGE_H
