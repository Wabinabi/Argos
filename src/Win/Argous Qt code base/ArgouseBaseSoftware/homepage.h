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

#include "tripdata.h"

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


private slots:

    // Buttons
    void on_pushButton_clicked();
    void on_ImportBtn_clicked();
    void on_BrowseBtn_clicked();

    //Recent Files
    //void newFile();
    //void open();
    //void save();
    //void saveAs();
    //void openRecentFile();
    void about();

private:
    Ui::HomePage *ui;

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

    // Import/process data
    //  We're assuming that the user imports one trip at a time
    //  and therefore the following repo-style variables will relate
    //  to only one trip
    QVector<QString> importedData;          // Data from PLY File
    QVector<DroneEvent> informEvents;          // All Verbose and Inform events
    // Processed data
    QVector<DroneEvent> verboseEvents;          // All Verbose and Inform events
    QVector<DroneEvent> emergencyEvents;    // Warning, Fatal, and Error events


    // Read coils for SCADA, essentially.
    bool tripEStopTriggered = false; // Flag to indicate that an E-Stop was hit
    bool tripOpOverrideTriggered = false; // Flag to indicate that an Operator Override occurrred


    // Statistics/Information
    int flightDuration; //
    int importDateTime;

    DroneSeriesData altitude;
    DroneSeriesData temperature;
    DroneSeriesData throttle;



    void readDroneStats(); // Read in the drone stats from the appdata dir
    void importPLY(QString dronePLYFile);  // Reads the PLY file from somewhere
    void importConf(QString droneConfFile); // Reads the config file as7.config
    void importLog(QString droneLogFile);  // Reads in the log as events

    //Read logs
    void readRecentFilesLog();
    QVector<QString> recentFiles;

    //Create Temp Working files
    void stashTempPLY();
    void stashTempEvents();

    //Close
    void closeEvent (QCloseEvent *event);

    //Recent Files
    void createActions();
    void createMenus();
    //void loadFile(const QString &fileName);
    //void saveFile(const QString &fileName);
    //void setCurrentFile(const QString &fileName);
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
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *separatorAct;
    QAction *TestRecentFile;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];
};

#endif // HOMEPAGE_H
