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

    // Import/process data
    QVector<QString> importedData;

    //Read logs
    void readRecentFilesLog();
    QVector<QString> recentFiles;

    //Create Temp Working files
    void generate_temp_PLY();

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
