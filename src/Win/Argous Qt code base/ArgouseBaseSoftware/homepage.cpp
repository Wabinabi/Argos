#include "homepage.h"
#include <QLineEdit>
#include <QFormLayout>

#include "tripdata.h"
#include "actions.h"

HomePage::HomePage(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::HomePage)
{
    ui->setupUi(this);

    // Uncomment to run in "Usability Test" mode. This means disabling unused features.
    UsabilityTestSim();

    readRecentFilesLog();
    readDroneStats();   

    createActions();
    createMenus();
    (void)statusBar();

    setWindowFilePath(QString());
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::UsabilityTestSim(){
    ui->DroneDisplay->setEnabled(false);
    //ui->FileBtn->setVisible(false);
    //ui->SettingsBtn->setVisible(false);
    //ui->HelpBtn->setVisible(false);
    //ui->ResetBtn->setVisible(false);
}

void HomePage::readRecentFilesLog()
{
    QString fileName = "FileLog.txt";
    QFile file(fileName);
    QMessageBox msg;

    recentFilesQueue.clear();
    ui->recentFiles->clear();

    //Reads text file log line by line, if no log file exists- it is built.
    if(!file.exists()){
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
    } else{

        QString line;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream stream(&file);
                while (!stream.atEnd()){
                    line = stream.readLine();
                    ui->recentFiles->addItem(line);

                    recentFilesQueue.enqueue(line);
                }
        file.close();
        }
    }
}


void HomePage::addRecentFile(QString recentFile) {




    while (recentFilesQueue.size() > 4) {
        recentFilesQueue.dequeue();
    }

    // Check if the file is already there
    bool fileAlreadyImported = false;
    for (int i = 0; i < recentFilesQueue.size(); i++ ) {
        if (recentFilesQueue[i] == recentFile) {
            fileAlreadyImported = true;
        }
    }
    recentFilesQueue.enqueue(recentFile);

    if (!fileAlreadyImported) {
        // Update the recent files text file
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

void HomePage::readDroneStats(){  
    QFile file("../ArgouseBaseSoftware/appdata/droneStats.txt");
    QString line;

    //file.open(QFile::ReadOnly | QFile::Text);
    //droneStats->setText(file.readAll());

    ui->droneStats->setReadOnly(true);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){

            line.append(stream.readLine()+"\n");
        }
        ui->droneStats->setText(line);
    }
    file.close();
}

void HomePage::on_pushButton_clicked()
{
    TripData tripData(this, &emergencyEvents, &verboseEvents, &informEvents, &altitude, &temperature, &throttle);
    tripData.setModal(false); //takes arguement for True/False, this determines whether the previous window can be accessed while the popup is open
    tripData.exec();
}


void HomePage::on_pushButton_4_clicked()
{
    // "Drone details" button clicked
    //  we create a new form layout


    if (!droneDetails->isVisible()) {
        if (importFailed) {
            QMessageBox msg;
            msg.setText("Please import drone data first");
            msg.exec();
        } else {
            droneDetails = new QDialog(this);
            QWidget *detailsScrollBox = new QWidget;

            detailsCloseButton = new QPushButton("Close");
            connect(detailsCloseButton, &QPushButton::released, this, &HomePage::on_droneDetailClose_clicked);

            detailSaveButton = new QPushButton("Save");
            connect(detailSaveButton, &QPushButton::released, this, &HomePage::on_droneDetailSave_clicked);

            detailResetButton = new QPushButton("Reset");
            connect(detailResetButton, &QPushButton::released, this, &HomePage::on_droneDetailReset_clicked);


            QFormLayout *layout = new QFormLayout();
            layout->addRow("Drone Configuration", new QLabel(""));



            QMap<QString, QString>::iterator i;
            for (i = droneConfig.begin(); i != droneConfig.end(); ++i) {
                QLineEdit *aValue = new QLineEdit(i.value());
                aValue->setReadOnly(false);

                layout->addRow(i.key(), aValue);
            }
            layout->addWidget(new QLabel(""));

            layout->addRow("Drone Details", new QLabel(""));

            for (i = droneDetailsMap.begin(); i != droneDetailsMap.end(); ++i) {
                QLineEdit *aValue = new QLineEdit(i.value());
                aValue->setReadOnly(false);

                layout->addRow(i.key(), aValue);
            }
            layout->addWidget(new QLabel(""));

            //layout->addWidget(detailsCloseButton);
            //B1 added widgets
            //layout->addWidget(detailSaveButton);
            //layout->addWidget(detailResetButton);
            QHBoxLayout *boxLayout = new QHBoxLayout();
            boxLayout->addWidget(detailResetButton);
            boxLayout->addWidget(detailSaveButton);
            boxLayout->addWidget(detailsCloseButton);

            QVBoxLayout *mainLayout = new QVBoxLayout();
            mainLayout->addLayout(layout);
            mainLayout->addLayout(boxLayout);

            detailsScrollBox->setLayout(mainLayout);
            QScrollArea *detailsScrollArea = new QScrollArea();
            detailsScrollArea->setStyleSheet("QWidget { border: 0px solid black };");
            detailsScrollArea->setWidgetResizable(true);
            detailsScrollArea->setWidget(detailsScrollBox);
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

void HomePage::on_droneDetailClose_clicked() {
    droneDetails->hide();
    detailsOpened = false;
}

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

void HomePage::on_droneDetailSave_clicked() {
    saveDroneDetails();
    on_droneDetailClose_clicked();

    QMessageBox msg;
    msg.setText("Drone details saved!");
    msg.exec();
}

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
    on_pushButton_4_clicked();
}

HomePage::DroneSeriesData HomePage::readColumnFromCSV(QString dataFile, QString colName)
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


    QFile file(dataFile);
    if (!file.open(QIODevice::ReadOnly)) {
        // throw some soort of error. A messagebox perhaps?
        qDebug() << "Could not open CSV File (" << dataFile<< ") for Column Series (" << colName << ") extraction";
    } else {
        QString headers = file.readLine();
        QVector<QString> headerList = headers.split(',');
        int colNumber;
        int foundCol = false;

        for (int i = 0; i < headerList.size(); i++) {
            if (headerList[i].contains(colName, Qt::CaseInsensitive)) {
                foundCol = true;
                colNumber = i;
            }
        }

        if (!foundCol) {
            // Could not locate target column in CSV
            qDebug() << "Target Column (" << colName << ") not found in CSV";

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

        seriesData.seriesName = colName,
        seriesData.data = data;

        return seriesData;
    }
}


HomePage::DroneSeriesData HomePage::readTempValues(QString dataFile)
{
    return readColumnFromCSV(dataFile, "temp");
}
HomePage::DroneSeriesData HomePage::readAltValues(QString dataFile)
{
    return readColumnFromCSV(dataFile, "us_1");
}

HomePage::DroneSeriesData HomePage::extractThrottleValues(QVector<DroneEvent> droneLogData)
{
    DroneEvent data;
    QStringList tokens;

    QVector<HomePage::DroneDataPoint> throttleData;

    for (int i = 0; i < droneLogData.size(); i++) {
        data = droneLogData[i];
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
             * As we only want throttle (Ch3), we take token 4 of this message.
             */
            tokens = data.message.split(QRegularExpression("\\s+"),Qt::SkipEmptyParts);

            HomePage::DroneDataPoint point {
                point.time = data.time,
                point.y = tokens[5].toFloat()
            };

            throttleData.append(point);
        }
    }
    DroneSeriesData throttleSeries;
    throttleSeries.seriesName = "Throttle Data";
    throttleSeries.data = throttleData;

    return throttleSeries;
}

void HomePage::on_ImportBtn_clicked()
{
    // Load explorer and browse for file if no file has been selected
    QString filename = ui->FileLocation->toPlainText();
    if(filename.isEmpty()){
        on_Browse_clicked();
        filename = ui->FileLocation->toPlainText();
    }

    configFileLocation = filename;

    if(filename.isEmpty()){
        return;
    }


    bool importLogSuccess = importLog(filename + "\\as7.log");
    bool importConfSuccess = importConf(filename + "\\as7.config", &droneConfig);
    bool importPLYSuccess = importPLY(filename + "\\data.csv");

    bool importTripsSuccess = importConf(filename + ".\\ArgouseBaseSoftware\\appdata\\tripStats.txt", &tripStats);
    bool importDroneStats = importConf(filename + ".\\ArgouseBaseSoftware\\appdata\\droneStats.txt", &runningStats);

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

        writeMapToFile("..\\ArgouseBaseSoftware\\appdata\\droneStats.txt", &runningStats);

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

void HomePage::writeMapToFile(QString dest, QMap<QString, QString> *map) {

    QFile file(dest);

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
        for (i = (*map).begin(); i != (*map).end(); ++i) {
            stream << i.key() << ":" << i.value() << '\n';

        }
        file.close();
    }
}


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



bool HomePage::importLog(QString droneLogFile){
    QFile file(droneLogFile);
    QMessageBox msg;

    bool isSuccessful = false; // Remembers if the import was successful

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

/* Removed in favour of importPLY()
 * importPLY saves into ../appdata/
void HomePage::stashTempPLY(){
    QString fileName = "temp/tempPLY.txt";
    QFile file(fileName);
    QMessageBox msg;


    if(!file.exists()){
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        file.close();
    }

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
          {
              // We're going to streaming text to the file
              QTextStream stream(&file);

              for (int i = 0; i < importedData.size(); ++i)
                  stream << importedData[i] << '\n';

              file.close();
          }
}
*/

//void HomePage::newFile()
//{
//    HomePage *other = new HomePage;
//    other->show();
//}

//void HomePage::open()
//{
//    QString fileName = QFileDialog::getOpenFileName(this);
//    if (!fileName.isEmpty())
//        loadFile(fileName);
//}

//void HomePage::save()
//{
//    if (curFile.isEmpty())
//        saveAs();
//    else
//        saveFile(curFile);
//}

void HomePage::exportPLY()
{

    QString fileName = "..\\ArgouseBaseSoftware\\appdata\\as7-map.ply";
    QFile file(fileName);

    if (fileName.isEmpty())
        return;

    saveFile(fileName);
}

//void HomePage::openRecentFile()
//{
//    QAction *action = qobject_cast<QAction *>(sender());
//    if (action)
//        loadFile(action->data().toString());
//}

void HomePage::help()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("../ArgouseBaseSoftware/appdata/docs/html/index.html"));
}

void HomePage::about()
{
    //UPDATE
   QMessageBox::about(this, tr("About Argous AS7 Base Software"),
            tr("The <b>Argous S7 Base Software</b> accompanies the AS7 drone. \
                Its purpose is to display drone diagnostic data, drone specs, "
               "and generate a 3D model from the collected data.\
                The repo for this project can be found at https://github.com/Wabinabi/Argous."));

}

void HomePage::createActions()
{
//    newAct = new QAction(tr("&New"), this);
//    newAct->setShortcuts(QKeySequence::New);
//    newAct->setStatusTip(tr("Create a new file"));
//    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

//    openAct = new QAction(tr("&Open..."), this);
//    openAct->setShortcuts(QKeySequence::Open);
//    openAct->setStatusTip(tr("Open an existing file"));
//    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

//    saveAct = new QAction(tr("&Save"), this);
//    saveAct->setShortcuts(QKeySequence::Save);
//    saveAct->setStatusTip(tr("Save the document to disk"));
//    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    exportAct = new QAction(tr("Export PLY..."), this);
    exportAct->setShortcuts(QKeySequence::SaveAs);
    exportAct->setStatusTip(tr("Export Map as PLY Document"));
    connect(exportAct, SIGNAL(triggered()), this, SLOT(exportPLY()));


    //for (int i = 0; i < MaxRecentFiles; ++i) {
//        recentFileActs[i] = new QAction(this);
//        recentFileActs[i]->setVisible(true);
        //connect(recentFileActs[i], SIGNAL(triggered()),
                //this, SLOT(openRecentFile()));
    //}

//    exitAct = new QAction(tr("E&xit"), this);
//    exitAct->setShortcuts(QKeySequence::Quit);
//    exitAct->setStatusTip(tr("Exit the application"));
//    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    helpAct = new QAction(tr("Open User Manual"), this);
    //helpAct->setShortcuts(QKeySequence::HelpContents);
    //new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));
    //helpAct->setStatusTip(tr("Opens the user manual in the default web browser"));
    QShortcut *shortcut = new QShortcut(QKeySequence(QKeySequence::HelpContents),this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(help()));
    connect(helpAct, SIGNAL(triggered()), this, SLOT(help()));

    aboutAct = new QAction(tr("About AS7 Base Software"), this);
    aboutAct->setStatusTip(tr("Opens the about box for base software"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void HomePage::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    //fileMenu->addAction(newAct);
    //fileMenu->addAction(openAct);
    //fileMenu->addAction(saveAct);
    fileMenu->addAction(exportAct);

    //fileMenu->addSeparator();

    //separatorAct = fileMenu->addSeparator();

    //for (int i = 0; i < recentFiles.length(); ++i)
        //fileMenu->addAction(recentFiles[i]);

    //fileMenu->addSeparator();

    //fileMenu->addAction(exitAct);
    //updateRecentFileActions();

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAct);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

//void HomePage::loadFile(const QString &fileName)
//{
//    QFile file(fileName);
//    if (!file.open(QFile::ReadOnly | QFile::Text)) {
//        QMessageBox::warning(this, tr("Recent Files"),
//                             tr("Cannot read file %1:\n%2.")
//                             .arg(fileName)
//                             .arg(file.errorString()));
//        return;
//    }

//    QTextStream in(&file);
//    QApplication::setOverrideCursor(Qt::WaitCursor);
//    textEdit->setPlainText(in.readAll());
//    QApplication::restoreOverrideCursor();

//    setCurrentFile(fileName);
//    statusBar()->showMessage(tr("File loaded"), 2000);
//}

void HomePage::saveFile(const QString &fileName)
{
    QFile file(fileName);
    /*
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Recent Files"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }


    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << textEdit->toPlainText();
    QApplication::restoreOverrideCursor();

    //setCurrentFile(fileName);
    statusBar()->showMessage(tr("File exported"), 2000);
    */

    //QString fileLocationStr = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
    //                                                "/");

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
        std::filesystem::copy(fileName.toStdString(), fileLocationStr.toStdString(), std::filesystem::copy_options::recursive);
        QMessageBox msg;
        msg.setText("File saved!");
        msg.exec();
    }

}

//void HomePage::setCurrentFile(const QString &fileName)
//{
//    curFile = fileName;
//    setWindowFilePath(curFile);

//    //QSettings settings;
//    files = settings.value("recentFileList").toStringList();
//    files.removeAll(fileName);
//    files.prepend(fileName);
//    while (files.size() > MaxRecentFiles)
//        files.removeLast();

//    settings.setValue("recentFileList", files);

////    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
////        HomePage *mainWin = qobject_cast<HomePage *>(widget);
////        if (mainWin)
////            mainWin->updateRecentFileActions();
////    }
//}



QString HomePage::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void HomePage::closeEvent (QCloseEvent *event)
{
    //Remove temp files
    std::filesystem::remove_all("temp");
}


void HomePage::on_Browse_clicked()
{
    QString fileLocationStr = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/",
                                                    QFileDialog::DontResolveSymlinks);
    ui->FileLocation->setPlainText(fileLocationStr);
}


void HomePage::on_recentFiles_itemClicked(QListWidgetItem *item)
{
    ui->FileLocation->setPlainText(item->text());
}


void HomePage::on_ExportButton_clicked()
{
    exportPLY();
}


void HomePage::on_HelpBtn_clicked()
{
    // Open the user manual
    help();
}


void HomePage::on_ResetBtn_clicked()
{
    ui->FileLocation->setPlainText("");
    importFailed = true; // Require user to import new data
    ui->droneStats->setPlainText("");
    QMessageBox msg;

    msg.setText("Imported data reset!");
    msg.exec();
}


void HomePage::on_FileBtn_clicked()
{
    on_ImportBtn_clicked();
}

