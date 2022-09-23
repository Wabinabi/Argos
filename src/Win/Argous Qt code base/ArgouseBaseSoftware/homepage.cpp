#include "homepage.h"
#include "ui_homepage.h"


HomePage::HomePage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomePage)
{
    ui->setupUi(this);
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::on_pushButton_clicked()
{
    TripData tripData;
    tripData.setModal(false); //takes arguement for True/False, this determines whether the previous window can be accessed while the popup is open
    tripData.exec();
}


void HomePage::on_ImportBtn_clicked()
{
        QString filename = ui->FileLocation->toPlainText();
        if(filename.isEmpty()){
            on_BrowseBtn_clicked();
            filename = ui->FileLocation->toPlainText();
        }

        QFile file(filename);
        QVector<QString> importedData;
        QMessageBox msg;

        if(!file.exists()){
            msg.setText("Please select a file");
            msg.exec();
        }else{
            QString line;

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
}


void HomePage::on_BrowseBtn_clicked()
{
    QString fileLocationStr = QFileDialog::getOpenFileName(this, tr("Open File"),"/path/to/file/",tr("Txt Files (*.txt)"));
    ui->FileLocation->insertPlainText(fileLocationStr);
}

