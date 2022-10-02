#include "homepage.h"
#include "ui_homepage.h"


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

void HomePage::on_ImportBtn_clicked()
{
    // Load explorer and browse for file if no file has been selected
    QString filename = ui->FileLocation->toPlainText();
    if(filename.isEmpty()){
        on_BrowseBtn_clicked();
        filename = ui->FileLocation->toPlainText();
    }



    bool importLogSuccess = importLog(filename + "\\as7.log");
    bool importConfSuccess = importConf(filename + "\\as7.config");
    bool importPLYSuccess = importPLY(filename + "\\data.csv");

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

    if (!importLogSuccess || !importConfSuccess || !importPLYSuccess) {msg.exec();}


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

    // Create a text file within the temp folder and populate with the PLY data from the drone
    stashTempPLY();

    //updateRecentFileActions(filename);
}

bool HomePage::importPLY(QString droneCSVFile){
    // Import the CSV file from the drone and generate a PLY file
    // Proceeds to call StashPLY to store the information

    // PLY file is saved to disk (probably in appdata) and referenced\
    //  by plotter.cpp
    QString dest = "..\\ArgouseBaseSoftware\\appdata\\as7-map.ply";


    bool isSuccessful = false; // Remembers if the import was successful

    qDebug() <<droneCSVFile << " - dest: " << dest << Qt::endl;
    DataTranslator translator = DataTranslator();

    translator.SetFilePath(droneCSVFile, dest);
    isSuccessful = translator.GenerateFile();

    return isSuccessful;
}

bool HomePage::importConf(QString droneConfFile){
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

                    droneConfigList.append(tokens[0] + " : "  + tokens[1]);
                    droneConfig[tokens[0]] = tokens[1];
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

// this ONLY copies the file over
//  processing is done in the previous importPLY file
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

void HomePage::stashTempEvents(){}



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


