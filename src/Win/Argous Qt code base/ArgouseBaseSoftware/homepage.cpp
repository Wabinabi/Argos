#include "homepage.h"
#include <QLineEdit>
#include <QFormLayout>


HomePage::HomePage(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::HomePage)
{
    ui->setupUi(this);

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

void HomePage::readRecentFilesLog()
{
    QString fileName = "FileLog.txt";
    QFile file(fileName);
    QMessageBox msg;

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
                    recentFiles.append(line);
                }
        file.close();
        }
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
    TripData tripData;
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
            droneDetails = new QWidget;
            droneDetails->setWindowModality(Qt::WindowModal);

            detailsCloseButton = new QPushButton("Close");
            connect(detailsCloseButton, &QPushButton::released, this, &HomePage::on_droneDetailClose_clicked);



            QFormLayout *layout = new QFormLayout(droneDetails);
            layout->addRow("Drone Configuration", new QLabel(""));



            QMap<QString, QString>::iterator i;
            for (i = droneConfig.begin(); i != droneConfig.end(); ++i) {
                QLineEdit *aValue = new QLineEdit(i.value());
                aValue->setReadOnly(true);

                layout->addRow(i.key(), aValue);
            }
            layout->addWidget(new QLabel(""));

            layout->addRow("Drone Details", new QLabel(""));

            for (i = droneDetailsMap.begin(); i != droneDetailsMap.end(); ++i) {
                QLineEdit *aValue = new QLineEdit(i.value());
                aValue->setReadOnly(true);

                layout->addRow(i.key(), aValue);
            }

            layout->addWidget(detailsCloseButton);

            droneDetails->resize(595, droneDetails->height());
            droneDetails->show();
            detailsOpened = true;
        }
    }
}

void HomePage::on_droneDetailClose_clicked() {
    droneDetails->hide();
    detailsOpened = false;
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
                    .time = cells[32].toInt(), // column number for millis
                    .y = cells[colNumber].toFloat()
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
                .time = data.time,
                .y = tokens[5].toFloat()
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
        on_BrowseBtn_clicked();
        filename = ui->FileLocation->toPlainText();
    }



    bool importLogSuccess = importLog(filename + "\\as7.log");
    bool importConfSuccess = importConf(filename + "\\as7.config", &droneConfig);
    bool importPLYSuccess = importPLY(filename + "\\data.csv");

    bool importDetailsSuccess = importConf("..\\ArgouseBaseSoftware\\appdata\\droneDetails.txt", &droneDetailsMap);

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
    }

    if (!importFailed) {
        // next we need altitude, throttle, and temp
        //  Altitude and Temp are given in the CSV
        //
        //  Throttle is slightly different as it's given in the log files



        // Extract throttle data from inform logs
        throttle = extractThrottleValues(verboseEvents);
        altitude = readAltValues(filename + "\\data.csv");
        temperature = readTempValues(filename + "\\data.csv");

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

    //updateRecentFileActions(filename);
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

void HomePage::on_BrowseBtn_clicked()
{
    //QString fileLocationStr = QFileDialog::getOpenFileName(this, tr("Open File"),"/path/to/file/",tr("Txt Files (*.txt)"));
    QString fileLocationStr = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/",
                                                    QFileDialog::DontResolveSymlinks);

    ui->FileLocation->insertPlainText(fileLocationStr);
}

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

//void HomePage::saveAs()
//{
//    QString fileName = QFileDialog::getSaveFileName(this);
//    if (fileName.isEmpty())
//        return;

//    saveFile(fileName);
//}

//void HomePage::openRecentFile()
//{
//    QAction *action = qobject_cast<QAction *>(sender());
//    if (action)
//        loadFile(action->data().toString());
//}

void HomePage::about()
{
   //UPDATE
   QMessageBox::about(this, tr("About Recent Files"),
            tr("The <b>Recent Files</b> example demonstrates how to provide a "
               "recently used file menu in a Qt application."));
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

//    saveAsAct = new QAction(tr("Save &As..."), this);
//    saveAsAct->setShortcuts(QKeySequence::SaveAs);
//    saveAsAct->setStatusTip(tr("Save the document under a new name"));
//    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));


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

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
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
    //fileMenu->addAction(saveAsAct);

    //fileMenu->addSeparator();

    //separatorAct = fileMenu->addSeparator();

    for (int i = 0; i < recentFiles.length(); ++i)
        fileMenu->addAction(recentFiles[i]);

    fileMenu->addSeparator();

    //fileMenu->addAction(exitAct);
    //updateRecentFileActions();

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
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

//void HomePage::saveFile(const QString &fileName)
//{
//    QFile file(fileName);
//    if (!file.open(QFile::WriteOnly | QFile::Text)) {
//        QMessageBox::warning(this, tr("Recent Files"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(fileName)
//                             .arg(file.errorString()));
//        return;
//    }

//    QTextStream out(&file);
//    QApplication::setOverrideCursor(Qt::WaitCursor);
//    out << textEdit->toPlainText();
//    QApplication::restoreOverrideCursor();

//    setCurrentFile(fileName);
//    statusBar()->showMessage(tr("File saved"), 2000);
//}

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

void HomePage::updateRecentFileActions(const QString &fullFileName)
{
    // Shift all elements downwards by working through the vector array backwards.
    for (int i = recentFiles.size() - 1; i >= 1; i--)
        recentFiles[i] = recentFiles[i-1];
    // Add the imported file details to most recent slot
    recentFiles[0] = fullFileName;

    // Updates the log file
    QFile file("FileLog.txt");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
          {
              // We're going to streaming text to the file
              QTextStream stream(&file);

              stream << recentFiles[0] << '\n' << recentFiles[1]
                        << '\n' << recentFiles[2] << '\n' << recentFiles[3] <<'\n';

              file.close();
          }
}

QString HomePage::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void HomePage::closeEvent (QCloseEvent *event)
{
    //Remove temp files
    std::filesystem::remove_all("temp");
}




