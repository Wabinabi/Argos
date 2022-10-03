#include "datatranslator.h"

DataTranslator::DataTranslator(QObject *parent)
    : QObject{parent}
{

}

DataTranslator::~DataTranslator(){}

void DataTranslator::SetFilePath(QString dataLocation,QString dataDestination){
    _dataPath = dataLocation;
    //_destPath = QFileInfo(dataDestination).absoluteDir().absolutePath();
    _destPath = dataDestination;
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
    return true;
}

bool DataTranslator::Calculate(){
      //QString today = QDate::currentDate().toString("yyyy-MM-dd");
     //QString filename = _destPath + "/" + today + ".txt";
     QString filename = _destPath;

        QFile file(filename);
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);

            for(int i = 0; i < _data.count(); i++){
                //Defined Data for Readability
                Drone.x = _data[i][_titles[0][2].toInt()].toDouble();
                Drone.y = _data[i][_titles[1][2].toInt()].toDouble();
                Drone.z = _data[i][_titles[2][2].toInt()].toDouble();
                Drone.angle = _data[i][_titles[3][2].toInt()].toDouble();

                //Loop through sensors and record values
                for(int j = 0; j < _points.count(); j++){
                    _points[j].value = _data[i][_titles[4 + j][2].toInt()].toDouble();
                    //negative vlaues
                    if (_points[j].sensor.contains("n")){
                        _points[j].value *= (-1);
                    }
                    if (!_points[j].sensor.contains("_X")){
                        _points[j].x = Drone.x;
                        _points[j].y = Drone.y;
                        _points[j].z = Drone.z;

                        if (_points[j].sensor.contains("_X")){
                            _points[j].x += _points[j].value;
                        }
                        if (_points[j].sensor.contains("_Y")){
                            _points[j].y += _points[j].value;
                        }
                        if (_points[j].sensor.contains("_Z")){
                            _points[j].z += _points[j].value;
                        }
                        stream << _points[j].x << " " << _points[j].y << " " << _points[j].z << "\n";
                        numberDataPoints += 3;
                    }
                }
            }
            file.close();

        } else return false;

    return true;
}

bool DataTranslator::OutputToFile(){
    return true;
}
