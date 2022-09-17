#include "datatranslator.h"

DataTranslator::DataTranslator(QObject *parent)
    : QObject{parent}
{

}

DataTranslator::~DataTranslator(){}

void DataTranslator::SetFilePath(QString dataLocation,QString dataDestination){
    _dataPath = dataLocation;
    _destPath = QFileInfo(dataDestination).absoluteDir().absolutePath();
}

bool DataTranslator::GenerateFile(){
    if (!ReadData()){
        qDebug() << "ERROR: Failed to read file";
        return false;
    }
    if (!AssignColumn()){
        qDebug() << "ERROR: Failed to find data columns";
        return false;
    }

    if(!Calculate()){
        qDebug() << "ERROR: Failed to calculate data";
        return false;
    }
    if(!OutputToFile()){
        qDebug() << "ERROR: Failed to calculate data";
        return false;
    }
    return true;
}

bool DataTranslator::ReadData(){
    QVector<QString> row;
    QFile file(_dataPath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << file.errorString();
            return false;
        }

        while (!file.atEnd()) {
            QString line = file.readLine();
            row = line.split(',');
            row.back() = row.back().trimmed();

            if (_headers.count() == 0){ _headers.append(row); }
            else { _data.append(row); }
        }

        qDebug() << _headers;
        qDebug() << _data;

        return true;
}

bool DataTranslator::AssignColumn(){
    int index = 0;
    int i = 0;
    Point hold;

    foreach (QVector<QString> value, _titles){
        index = _headers.indexOf(value[1]);
        if (index < 0){return false;}
        _titles[i][2] = QString::number(index);
        if(value[0].contains("US_")){
            hold.sensor = value[0];
            hold.value = 0; hold.x = 0; hold.y = 0; hold.z = 0;
            _points.append(hold);
        }
        i++;
    }

    qDebug() << _titles;
    return true;
}

bool DataTranslator::Calculate(){
    }
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QString filename = _destPath + "/" + today + ".txt";

        QFile file(filename);
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            stream << today << "\n";
            file.close();
        } else return false;

    return true;
}

bool DataTranslator::OutputToFile(){
    return true;
}
