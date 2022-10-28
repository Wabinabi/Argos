/****************************************************
 * Description: This is the homepage. It provides the following features:
 *      Import Data (browse for directory/recent files and import)
 *      Menu bar with basic features (help, file, about, preferences)
 *      Access to drone config and specs
 *      Running drone stats display
 *      Access to drone trip data
 *      Clear data feature
 *      Recent files
 *
 * Author/s: Monique Kuhn, Jimmy Trac, Bi Wan Low
****************************************************/

#include "homepage.h"
#include "datatranslator.h"
#include "tripdata.h"

#include <QLineEdit>
#include <QFormLayout>

/***************************************************/

HomePage::HomePage(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::HomePage)
{
    ui->setupUi(this);
    ui->DroneDisplayBlack->hide();

    Parent = parent;

    //Uncomment to run in "Usability Test" mode. This means disabling unused features.
    //UsabilityTestSim();

    /*Read necessary files and info to display on homepage*/
    readRecentFilesLog();
    readDroneStats();   

    /*Setup the page's contents*/
    createActions();
    createMenus();
    (void)statusBar();

    setWindowFilePath(QString());
}

HomePage::~HomePage()
{
    delete ui;
}

/***************************************************/

/*Update this function to hide features that aren't to be tested in the usability mode
Comment out this function in the main function to disable usability testing mode!*/
void HomePage::usabilityTestSim(){
    ui->FileBtn->setVisible(false);
    ui->HelpBtn->setVisible(false);
    ui->ResetBtn->setVisible(false);
}

/*Read recent files and populate the recent files list display*/
void HomePage::readRecentFilesLog()
{
    QString fileName = "FileLog.txt";
    QFile file(fileName);
    QMessageBox msg;

    recentFilesQueue.clear();
    ui->RecentFilesLst->clear();

    /*Reads text file log line by line, if no log file exists- it is built.*/
    if(!file.exists()){
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
    } else{
        QString line;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream stream(&file);
                while (!stream.atEnd()){
                    line = stream.readLine();
                    ui->RecentFilesLst->addItem(line);
                    recentFilesQueue.enqueue(line);
                }
        file.close();
        }
    }
}

/*Upon import, this function is called. It adds the imported file to the "recent files" text*/
void HomePage::addRecentFile(QString _recentFile) {
    while (recentFilesQueue.size() > 4) {
        recentFilesQueue.dequeue();
    }

    /*Check if the file is already there*/
    bool fileAlreadyImported = false;
    for (int i = 0; i < recentFilesQueue.size(); i++ ) {
        if (recentFilesQueue[i] == _recentFile) {
            fileAlreadyImported = true;
        }
    }
    recentFilesQueue.enqueue(_recentFile);

    if (!fileAlreadyImported) {
        /*Update the recent files text file*/
        QFile file("FileLog.txt");

        if(!file.exists()){
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream out(&file);
            file.close();
        }

        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            for (int i = 0; i < recentFilesQueue.size(); i++ ) {
                stream << recentFilesQueue[i] << '\n';
            }
            file.close();
        }
        readRecentFilesLog();
    }
}

/*Clears all recent files from the text file*/
void HomePage::clearRecentFiles() {
    QString fileName = "FileLog.txt";
    QFile file(fileName);

    recentFilesQueue.clear();
    ui->RecentFilesLst->clear();

    file.open(QFile::WriteOnly|QFile::Truncate);
}

/*Reads all the running drone stats and displays it on the page*/
void HomePage::readDroneStats(){  
    QFile file("../ArgouseBaseSoftware/appdata/DroneStatsTxt.txt");
    QString line, htmlLine,textBlock;
    QStringList tokens;

    ui->DroneStatsTxt->setReadOnly(true);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){
            line = stream.readLine();
            tokens = line.split(QRegularExpression(":"));
            htmlLine = "<html><b>" + tokens[0] + ": </b></html>" + tokens[1] + "<html><br></html>";

            textBlock.append(htmlLine);
        }
        ui->DroneStatsTxt->setText(textBlock);
    }
    file.close();
}

/*Loads the trip data page and passes it necessary data.
 *Blocks trip data from being opened if data is not present.*/
void HomePage::on_ViewTripDataBtn_clicked()
{
    if (!altitude.data.empty()){
           TripData tripData(this, &emergencyEvents, &verboseEvents, &informEvents, &altitude, &temperature, &throttle);
           tripData.setModal(false); //takes arguement for True/False, this determines whether the previous window can be accessed while the popup is open
           tripData.exec();
       }
       else{
           QMessageBox msg;
           msg.setText("Please import Data");
           msg.exec();
       }
}

/*This function generates a popup display that depicts the drone specs and data.
* The display features a Save, Close, and Reset button that caters for editing capabilities*/
void HomePage::on_ViewDroneDetsBtn_clicked()
{
    /*Checks if drone data file exists*/
    if (!droneDetails->isVisible()) {
        if (importFailed) {
            QMessageBox msg;
            msg.setText("Please import drone data first");
            msg.exec();
        } else {
            droneDetails = new QDialog(this);
            QWidget *detailsScrollBox = new QWidget;

            /*If data exists, the widget is drawn with Close, Sace, and Reset file*/
            detailsCloseButton = new QPushButton("Close");
            connect(detailsCloseButton, &QPushButton::released, this, &HomePage::on_droneDetailClose_clicked);

            detailSaveButton = new QPushButton("Save");
            connect(detailSaveButton, &QPushButton::released, this, &HomePage::on_droneDetailSave_clicked);

            detailResetButton = new QPushButton("Reset");
            connect(detailResetButton, &QPushButton::released, this, &HomePage::on_droneDetailReset_clicked);

            /*Layout generation -title, and each row of details from the text file*/
            QFormLayout *layout = new QFormLayout();

            /*Draw all drone config variable names*/
            layout->addRow("Drone Configuration", new QLabel(""));
            QMap<QString, QString>::iterator i;
            for (i = droneConfig.begin(); i != droneConfig.end(); ++i) {
                QLineEdit *aValue = new QLineEdit(i.value());
                aValue->setReadOnly(false);
                layout->addRow(i.key(), aValue);
            }

            /*Draw all drone config variable values*/
            layout->addWidget(new QLabel(""));
            layout->addRow("Drone Details", new QLabel(""));
            for (i = droneDetailsMap.begin(); i != droneDetailsMap.end(); ++i) {
                QLineEdit *aValue = new QLineEdit(i.value());
                aValue->setReadOnly(false);
                layout->addRow(i.key(), aValue);
            }
            layout->addWidget(new QLabel(""));

            /*Draw buttons*/
            QHBoxLayout *boxLayout = new QHBoxLayout();
            boxLayout->addWidget(detailResetButton);
            boxLayout->addWidget(detailSaveButton);
            boxLayout->addWidget(detailsCloseButton);

            QVBoxLayout *mainLayout = new QVBoxLayout();
            mainLayout->addLayout(layout);
            mainLayout->addLayout(boxLayout);

            /*dynamic scaling + scroll to support dynamic scaling and varying window size*/
            detailsScrollBox->setLayout(mainLayout);
            QScrollArea *detailsScrollArea = new QScrollArea();
            detailsScrollArea->setStyleSheet("QWidget { border: 0px solid black };");
            detailsScrollArea->setWidgetResizable(true);
            detailsScrollArea->setWidget(detailsScrollBox);

            /*General layout*/
            droneDetails->setLayout(new QVBoxLayout);
            droneDetails->layout()->addWidget(detailsScrollArea);
            droneDetails->setWindowTitle("Argous S7 Drone Details");
            droneDetails->resize(595, 450);
            droneDetails->setWindowModality(Qt::WindowModal);
            droneDetails->exec();
            detailsOpened = true;
        }
    }
}

/*Suspends/closes drone details popup*/
void HomePage::on_droneDetailClose_clicked() {
    droneDetails->hide();
    detailsOpened = false;
}

/*Saves the drone details that are edited by the user*/
void HomePage::saveDroneDetails() {
    QList items = droneDetails->children();

    for (int i = 0; i < items.count(); i++){
        if (qobject_cast<QLabel*>(items[i]) != NULL){
            QString labelText = qobject_cast<QLabel*>(items[i])->text();

            qDebug() << "Finding: " << labelText;

            if (droneConfig.find(labelText) != droneConfig.end()) {
                droneConfig[labelText] = qobject_cast<QLineEdit*>(items[i+1])->text();
            }

            if (droneDetailsMap.find(labelText) != droneDetailsMap.end()) {
                droneDetailsMap[labelText] = qobject_cast<QLineEdit*>(items[i+1])->text();
            }
        }
    }

    writeMapToFile(configFileLocation + "\\as7.config", &droneConfig);
    writeMapToFile("..\\ArgouseBaseSoftware\\appdata\\droneDetails.txt", &droneDetailsMap);
}

/*Calls save function when drone save button clicked. Provides user feedback*/
void HomePage::on_droneDetailSave_clicked() {
    saveDroneDetails();
    on_droneDetailClose_clicked();

    QMessageBox msg;
    msg.setText("Drone details saved!");
    msg.exec();
}

/*Calls drone details reset function when drone save button clicked. Provides user feedback*/
void HomePage::on_droneDetailReset_clicked() {
    QMap<QString, QString>::iterator i;
    for (i = defaultDroneDetailsMap.begin(); i != defaultDroneDetailsMap.end(); ++i) {
        droneDetailsMap[i.key()] = i.value();
    }

    writeMapToFile("..\\ArgouseBaseSoftware\\appdata\\droneDetails.txt", &droneDetailsMap);
    on_droneDetailClose_clicked();

    QMessageBox msg;
    msg.setText("Drone details reset!");
    msg.exec();
    on_ViewDroneDetsBtn_clicked();
}

/*Reads column name data from the CSV file from the passed through data*/
HomePage::DroneSeriesData HomePage::readColumnFromCSV(QString _dataFile, QString _colName)
{
    /* Returns a column from the CSV, similar to the data translator
     *
     * - Read the CSV as as a stream
     * - Extract by ,
     * - Get the column number
     * - read for those columns
     * - don't worry about efficiency for now
     */

    QVector<HomePage::DroneDataPoint> data;
    data.clear();

    QFile file(_dataFile);
    if (!file.open(QIODevice::ReadOnly)) {
        // throw some soort of error. A messagebox perhaps?
        qDebug() << "Could not open CSV File (" << _dataFile<< ") for Column Series (" << _colName << ") extraction";
    } else {
        QString headers = file.readLine();
        QVector<QString> headerList = headers.split(',');
        int colNumber;
        int foundCol = false;

        for (int i = 0; i < headerList.size(); i++) {
            if (headerList[i].contains(_colName, Qt::CaseInsensitive)) {
                foundCol = true;
                colNumber = i;
            }
        }

        if (!foundCol) {
            // Could not locate target column in CSV
            qDebug() << "Target Column (" << _colName << ") not found in CSV";

        } else {
            while (!file.atEnd()) {
                QString row = file.readLine();
                QVector<QString> cells = row.split(',');

                // Get the ColNumber element and add as a data point
                HomePage::DroneDataPoint point {
                    point.time = cells[32].toInt(), // column number for millis
                    point.y = cells[colNumber].toFloat()
                };

                data.append(point);
            }
        }

        HomePage::DroneSeriesData seriesData;

        seriesData.seriesName = _colName,
        seriesData.data.clear();
        seriesData.data = data;

        return seriesData;
    }
}

/*Reads column temp data from the CSV file from the passed through data*/
HomePage::DroneSeriesData HomePage::readTempValues(QString _dataFile)
{
    return readColumnFromCSV(_dataFile, "temp");
}

/*Reads column altitude data from the CSV file from the passed through data*/
HomePage::DroneSeriesData HomePage::readAltValues(QString _dataFile)
{
    return readColumnFromCSV(_dataFile, "us_1");
}

/*Reads column throttle data from the CSV file from the passed through data*/
HomePage::DroneSeriesData HomePage::extractThrottleValues(QVector<DroneEvent> _droneLogData)
{
    DroneEvent data;
    QStringList tokens;

    QVector<HomePage::DroneDataPoint> throttleData;
    throttleData.clear();

    for (int i = 0; i < _droneLogData.size(); i++) {
        data = _droneLogData[i];
        if (data.message.contains("DATA:", Qt::CaseInsensitive)) {
            // This is a drone log event in the form:
            // DATA: 0 TX: <16 TX Chs> RX: <16 RX Chs>
            //
            // The tokens are therefore:
            /*
             * 0: DATA:
             * 1: Data Value    (0 = Malformed Frame Received, 1 = Valid Frame received)
             * 2: Tx:           (Tx Marker)
             * 3: Tx Ch 1
             * 4: Tx Ch 2
             * 5: Tx Ch 3
             * 6..17 Tx Channels
             * 19: RX:          (Rx Marker)
             * 20..35 Rx Channels
             * As we only want throttle (Ch3), we take token 5 of this message.
             */
            tokens = data.message.split(QRegularExpression("\\s+"),Qt::SkipEmptyParts);

            // Only add the point of data is good

            if (tokens[1] != "0") {
                HomePage::DroneDataPoint point {
                    point.time = data.time,
                    point.y = tokens[5].toFloat()
                };
                throttleData.append(point);
            }

        }
    }
    DroneSeriesData throttleSeries;
    throttleSeries.seriesName = "Throttle Data";\
    throttleSeries.data.clear();
    throttleSeries.data = throttleData;

    return throttleSeries;
}

/*imports data when import button is clicked. Gives error messaging if directory not chosen or data missing.*/
void HomePage::on_ImportBtn_clicked()
{
    // Load explorer and browse for file if no file has been selected
    QString filename = ui->FileLocationTxt->toPlainText();
    if(filename.isEmpty()){
        on_BrowseBtn_clicked();
        filename = ui->FileLocationTxt->toPlainText();
    }

    configFileLocation = filename;

    if(filename.isEmpty()){
        return;
    }

    bool importLogSuccess = importLog(filename + "\\as7.log");
    bool importConfSuccess = importConf(filename + "\\as7.config", &droneConfig);
    bool importPLYSuccess = importPLY(filename + "\\data.csv");

    //bool importTripsSuccess = importConf(filename + ".\\ArgouseBaseSoftware\\appdata\\tripStats.txt", &tripStats);
    //bool importDroneStats = importConf(filename + ".\\ArgouseBaseSoftware\\appdata\\DroneStatsTxt.txt", &runningStats);

    bool importDetailsSuccess = importConf("..\\ArgouseBaseSoftware\\appdata\\droneDetails.txt", &droneDetailsMap);
    importDetailsSuccess = importConf("..\\ArgouseBaseSoftware\\appdata\\defaultDroneDetails.txt", &defaultDroneDetailsMap);


    // Create error message
    QMessageBox msg;
    QString errorMsg = "";

    // Import failed flags are set here
    //  in case there are additional checks for import failed
    if (!importLogSuccess) {
        errorMsg += "Log file not found in the directory! Is as7.log missing or misplaced?\n";
        importFailed = true;
    }
    if (!importConfSuccess) {
        errorMsg += "Config file not found in the directory! Is as7.config missing or misplaced?\n";
        importFailed = true;
    }
    if (!importPLYSuccess) {
        errorMsg += "CSV file not found in the directory! Is data.csv missing or misplaced?\n";
        importFailed = true;
    }

    if (!importDetailsSuccess) {
        errorMsg += "Details of the drone could not be loaded! Is droneDetails.txt missing from the appdata directory?\n";
        importFailed = true;
    }

    if (!importLogSuccess || !importConfSuccess || !importPLYSuccess) {
        msg.setText(errorMsg);
        msg.exec();
    } else {
        errorMsg = "Import Successful!";
        importFailed = false;
        msg.setText(errorMsg);
        msg.exec();

        // TODO: read lifetimes and increment here (e.g. total trip duration (ms)

    }

    if (!importFailed) {

        // Extract throttle data from inform logs
        throttle = extractThrottleValues(verboseEvents);
        altitude = readAltValues(filename + "\\data.csv");
        temperature = readTempValues(filename + "\\data.csv");

        // Extract trip details
        // Get sample rate from config
        sampleRate = droneConfig["SAMPLE_RATE"].toInt();

        // Get average temperature by summing and dividing across temperature series
        float totalTemp = 0;
        for (int i = 0; i < temperature.data.size(); i++) {
            totalTemp+=temperature.data[i].y;
        }

        averageTemp = totalTemp / temperature.data.size();

        // Get the number of events by looking at the size of the vector arrays
        numberCriticalEvents = emergencyEvents.size();
        numberEvents = verboseEvents.size() + informEvents.size() + emergencyEvents.size();

        // We should have all of the information we need to update trip data and running data
        // the trip data we write fresh intot he dict and store

        tripStats["Number of Points During Flight"] = QString::number(numberDataPoints);
        tripStats["Duration of flight (s)"] = QString::number(tripDuration/1000);
        tripStats["Sample Rate (Hz)"] = QString::number(sampleRate);
        tripStats["Average Temp (c)"] = QString::number(averageTemp);
        tripStats["Number of Events"] = QString::number(numberEvents);
        tripStats["Number of Critical Events"] = QString::number(numberCriticalEvents);

        writeMapToFile("..\\ArgouseBaseSoftware\\appdata\\tripStats.txt", &tripStats);

        // the running data we read from what's existing, update, and store

        int lifetimeStarts = runningStats["Lifetime Starts"].toInt();
        int totalTripDur = runningStats["Total Trip Duration (s)"].toInt();


        lifetimeStarts += 1;
        totalTripDur += tripDuration/1000;

        runningStats["Lifetime Starts"] = QString::number(lifetimeStarts);
        runningStats["Total Trip Duration (s)"] = QString::number(totalTripDur);
        runningStats["Date of Last Trip Imported"] =QDate::currentDate().toString("yyyy-MM-dd");
        runningStats["Last Maintenance Date"] = "2022-10-03";

        writeMapToFile("..\\ArgouseBaseSoftware\\appdata\\DroneStatsTxt.txt", &runningStats);

        //updateRecentFileActions(filename);
        addRecentFile(filename);
    }



    /*Removed by Jimmy as the following has been moved to new functions
    // Reads the selected file and stores each line
    // Further Processing will need to be performed here! Likely need to create a new function to call
    if(!file.exists()){
        msg.setText("Please select a file");
        msg.exec();
    }else{
        //importPLY();
        //importConf();
        //importLog();




        // This block of text is temporarily substituting PLY processor

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream stream(&file);
                while (!stream.atEnd()){
                    line = stream.readLine();
                    importedData.append(line);
                }
        msg.setText("Import complete!");
        msg.exec();

        file.close();
        }

    }
    */

    // Create a temp directory where we store temp files
    std::filesystem::create_directory("temp");

    // Update drone stats
    readDroneStats();


}

/*Writes data map to file*/
void HomePage::writeMapToFile(QString _dest, QMap<QString, QString> *_map) {

    QFile file(_dest);

    if(!file.exists()){
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        file.close();
    }

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // We're going to streaming text to the file
        QTextStream stream(&file);

        QMap<QString, QString>::iterator i;
        for (i = (*_map).begin(); i != (*_map).end(); ++i) {
            stream << i.key() << ":" << i.value() << '\n';

        }
        file.close();
    }
}

/*Generates the PLY file upon import*/
bool HomePage::importPLY(QString droneCSVFile){
    // Import the CSV file from the drone and generate a PLY file
    // Proceeds to call StashPLY to store the information

    // PLY file is saved to disk (probably in appdata) and referenced\
    //  by plotter.cpp
    QString dest = "..\\ArgouseBaseSoftware\\appdata\\as7-map.ply";


    bool isSuccessful = false; // Remembers if the import was successful

    DataTranslator translator = DataTranslator();

    translator.SetFilePath(droneCSVFile, dest);
    isSuccessful = translator.GenerateFile();

    numberDataPoints = translator.numberDataPoints;

    return isSuccessful;
}

/*Imports the drone configuration details*/
bool HomePage::importConf(QString droneConfFile, QMap<QString, QString> *dest){
    //store into vector arrays
    QFile file(droneConfFile);

    bool isSuccessful = false; // Remembers if the import was successful

    if(!file.exists()){
        isSuccessful = false;
    }else{
        QString line;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream stream(&file);

                while (!stream.atEnd()){
                    line = stream.readLine();
                    QStringList tokens;
                    // Split the line by :
                    // Token 0 key, Token 1 Value
                    tokens = line.split(QRegularExpression("[:]"), Qt::SkipEmptyParts);

                    (*dest)[tokens[0]] = tokens[1];
                }
        file.close();
        isSuccessful = true;
        }
    }
    return isSuccessful;
}

/*Imports the drone events log details*/
bool HomePage::importLog(QString droneLogFile){
    QFile file(droneLogFile);
    QMessageBox msg;

    bool isSuccessful = false; // Remembers if the import was successful

    verboseEvents.clear();
    informEvents.clear();
    emergencyEvents.clear();

    // Reads the selected file and stores each line
    // Further Processing will need to be performed here! Likely need to create a new function to call
    if(!file.exists()){
        isSuccessful = false;
    }else{
        QString line;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream stream(&file);
                line = stream.readLine(); // Skip the first line

                while (!stream.atEnd()){
                    line = stream.readLine();
                    QStringList tokens;
                    DroneEvent event;

                    // Split the line by [ and ] to get the time and messages
                    tokens = line.split(QRegularExpression("[[]|[]]|ms"), Qt::SkipEmptyParts);
                    // Tokens[0] will always be the time, the second will always be Type

                    event.time = tokens[0].toInt();
                    event.errorType = tokens[1].trimmed();

                    //int offset = line.indexOf("]");
                    //event.message = line.sliced(offset);
                    event.message = tokens[2];

                    // check the event type and import into emergency events or event data
                    // We'll do string comparisons here to reduce the effort and potential
                    //  issues related to casting strings into types
                    if ((event.errorType == "Fatal" ) || (event.errorType == "Error")) {
                        emergencyEvents.append(event);
                    } else if ((event.errorType == "Inform") || (event.errorType == "Warn")) {
                        informEvents.append(event);
                    } else {
                        // A verbose or identified event
                        verboseEvents.append(event);
                    }
                }
        file.close();
        isSuccessful = true;

        DroneEvent finalEvent = verboseEvents.last();
        tripDuration = finalEvent.time;
        }
    }
    return isSuccessful;
}

/*Saves the imported and generated PLY as a .PLY for the user*/
void HomePage::exportPLY()
{
    QString fileName = "..\\ArgouseBaseSoftware\\appdata\\as7-map.ply";
    QFile file(fileName);

    if (fileName.isEmpty())
        return;

    saveFile(fileName);
}

/*Generates help*/
void HomePage::help()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("../ArgouseBaseSoftware/appdata/docs/html/index.html"));
}

/*Re-stylise to darkmode by updating apps style sheet*/
void HomePage::darkMode(){
    ui->DroneDisplayBlack->hide();
    ui->DroneDisplayWhite->show();

    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::black);
    this->setPalette(pal);

    QFile styleSheetFile("../ArgouseBaseSoftware/stylesheets/DarkMode.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleSheetFile.readAll());
    appParent->setStyleSheet(styleSheet);
}

/*Re-stylise to lightmode by updating apps style sheet*/
void HomePage::lightMode(){
    ui->DroneDisplayBlack->show();
    ui->DroneDisplayWhite->hide();


    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::black);
    this->setPalette(pal);

    QFile styleSheetFile("../ArgouseBaseSoftware/stylesheets/LightMode.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleSheetFile.readAll());
    appParent->setStyleSheet(styleSheet);
}

/*Generates about popup*/
void HomePage::about()
{
       QMessageBox::about(this, tr("About Argous AS7 Base Software"),
                tr("The <b>Argous S7 Base Software</b> accompanies the AS7 drone. \
                    Its purpose is to display drone diagnostic data, drone specs, "
                   "and generate a 3D model from the collected data.\
                    The repo for this project can be found at <a href=\https://github.com/Wabinabi/Argous\>Argous GitHub</a>."));
}

/*Generates all the actions that are going to be used in the menu*/
void HomePage::createActions()
{
    exportAct = new QAction(tr("Export PLY..."), this);
    exportAct->setShortcuts(QKeySequence::SaveAs);
    exportAct->setStatusTip(tr("Export Map as PLY Document"));
    connect(exportAct, SIGNAL(triggered()), this, SLOT(exportPLY()));

    helpAct = new QAction(tr("Open User Manual"), this);
    helpAct->setStatusTip(tr("Opens the user manual in the default web browser"));
    connect(helpAct, SIGNAL(triggered()), this, SLOT(help()));

    QShortcut *shortcut = new QShortcut(QKeySequence(QKeySequence::HelpContents),this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(help()));
    connect(helpAct, SIGNAL(triggered()), this, SLOT(help()));

    aboutAct = new QAction(tr("About AS7 Base Software"), this);
    aboutAct->setStatusTip(tr("Opens the about box for base software"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


    darkModeAct = new QAction(tr("Dark Mode"), this);
    darkModeAct->setStatusTip(tr("View software in dark mode"));
    connect(darkModeAct, SIGNAL(triggered()), this, SLOT(darkMode()));

    lightModeAct = new QAction(tr("Light Mode"), this);
    lightModeAct->setStatusTip(tr("View software in light mode"));
    connect(lightModeAct, SIGNAL(triggered()), this, SLOT(lightMode()));
}

/*Creates and populates menu bar*/
void HomePage::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exportAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAct);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    helpMenu = menuBar()->addMenu(tr("&Preferences"));
    helpMenu->addAction(darkModeAct);
    helpMenu->addAction(lightModeAct);
}

/*This is called by the export PLY function. It allows the user to choose save location*/
void HomePage::saveFile(const QString &_fileName)
{
    QFile file(_fileName);

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter("3D Point Cloud (*.PLY)");
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    //QString fileLocationStr = QFileDialog::getOpenFileName(this, tr("Save 3D Map"), "map.ply", tr("Mapping File (*.PLY)"));
    QString fileLocationStr;
    if (dialog.exec()) {
        fileLocationStr = dialog.selectedFiles()[0];
    }
    if (fileLocationStr.toStdString() != "") {
        std::filesystem::copy(_fileName.toStdString(), fileLocationStr.toStdString(), std::filesystem::copy_options::recursive);
        QMessageBox msg;
        msg.setText("File saved!");
        msg.exec();
    }

}

/*Returns stripped name from fill file name*/
QString HomePage::strippedName(const QString &_fullFileName)
{
    return QFileInfo(_fullFileName).fileName();
}

/*Remove temp files upon closing the window*/
void HomePage::closeEvent (QCloseEvent *_event)
{
    //Remove temp files
    std::filesystem::remove_all("temp");
}

/*Opens windows explorer browsing feature upon clicking '...'*/
void HomePage::on_BrowseBtn_clicked()
{
    QString fileLocationStr = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/",
                                                    QFileDialog::DontResolveSymlinks);
    ui->FileLocationTxt->setPlainText(fileLocationStr);
}

/*If recent file in fecents files is clicked, the file location directory is updated*/
void HomePage::on_RecentFilesLst_itemClicked(QListWidgetItem *_item)
{
    ui->FileLocationTxt->setPlainText(_item->text());
}

/*Calls export function when export button clicked*/
void HomePage::on_ExportBtn_clicked()
{
    exportPLY();
}

/*Opens user manual*/
void HomePage::on_HelpBtn_clicked()
{
    help();
}

/*When the reset button is clicked. The file location directory and data is cleared.*/
void HomePage::on_ResetBtn_clicked()
{
    ui->FileLocationTxt->setPlainText("");
    importFailed = true; // Require user to import new data
    ui->DroneStatsTxt->setPlainText("");
    QMessageBox msg;

    verboseEvents.clear();
    informEvents.clear();
    emergencyEvents.clear();

    clearRecentFiles();

    msg.setText("Imported data reset!");
    msg.exec();
}

/*Alternative import file button*/
void HomePage::on_FileBtn_clicked()
{
    on_ImportBtn_clicked();
}

/*Clears file location directory but NOT the data*/
void HomePage::on_ClearBtn_clicked()
{
    ui->FileLocationTxt->setPlainText("");
    importFailed = true;
}

