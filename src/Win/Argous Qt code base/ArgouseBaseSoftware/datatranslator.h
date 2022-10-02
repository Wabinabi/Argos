#ifndef DATATRANSLATOR_H
#define DATATRANSLATOR_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDate>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QMultiMap>


class DataTranslator : public QObject
{
    Q_OBJECT
public:
    explicit DataTranslator(QObject *parent = nullptr);
    ~DataTranslator();

    void SetFilePath(QString dataLocation,QString dataDestination);
    bool GenerateFile();

    int numberDataPoints = 0;

private:
    QString _dataPath;
    QString _destPath;

    QVector<QString> _headers;
    QVector<QVector<QString>> _data;

    //InteralString, ExternalString, cloumn
    //InternalString shouldn't be changed
    QVector<QVector<QString>> _titles{
        {"DronePos_X"  , " filt_pos_x"  , "-1"}, //DronePos_X
        {"DronePos_Y"  , " filt_pos_y"  , "-1"}, //DronePos_Y
        {"DronePos_Z"  , " filt_pos_z"  , "-1"}, //DronePos_Z
        {"DroneHeading", " heading"     , "-1"}, //DroneHeading
        {"US_Xp"       , " us_0"        , "-1"}, //"US_0" < - Test all US sensors on drone for confirmation.
        {"US_Xn"       , " us_3"        , "-1"}, //"US_3"
        {"US_Yp"       , " us_2"        , "-1"}, //"US_2"
        {"US_Yn"       , " us_5"        , "-1"}, //"US_5"
        {"US_Zp"       , " us_4"        , "-1"}, //"US_4"
        {"US_Zn"       , " us_1"        , "-1"}  //"US_1"
    };

    struct Point{
        QString sensor;
        double value;
        double x;
        double y;
        double z;
    };
    QVector<Point> _points;

    struct {
        double x;
        double y;
        double z;
        double angle;
    } Drone;

    bool ReadData();
    bool AssignColumn();
    bool Calculate();
    bool OutputToFile();
};

#endif // DATATRANSLATOR_H
