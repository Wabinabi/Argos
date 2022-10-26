#ifndef PLOTTER_H
#define PLOTTER_H

#include <QtDataVisualization/q3dscatter.h>
#include <QtDataVisualization/qabstract3dseries.h>
#include <QtGui/QFont>
#include <QtDataVisualization/qscatterdataproxy.h>
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/qscatter3dseries.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtCore/qmath.h>
#include <QtCore/qrandom.h>
#include <QtWidgets/QComboBox>
#include <QFileDialog>
#include <QMainWindow>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>

class Plotter : public QObject
{
    Q_OBJECT
public:
    //Plotter();
    explicit Plotter(Q3DScatter* scatter);
    ~Plotter();

    void addData();
    void changeStyle();
    void changePresetCamera();
    void changeLabelStyle();
    void changeFont(const QFont& _font);
    void setBackgroundEnabled(int _enabled);
    void setGridEnabled(int _enabled);
    void setSmoothDots(int _smooth);
    void toggleItemCount();
    void start();
    void closeEvent(QCloseEvent *event);

public Q_SLOTS:
    void changeStyle(int _style);
    void changeTheme(int _theme);
    void changeShadowQuality(int _quality);
    void shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality _shadowQ);

Q_SIGNALS:
    void backgroundEnabledChanged(bool _enabled);
    void gridEnabledChanged(bool _enabled);
    void shadowQualityChanged(int _quality);
    void fontChanged(const QFont& _font);

private:
    QVector3D PLYPoint(QString line);

    QVector3D randVector();
    Q3DScatter* graph;

    int fontSize;
    QAbstract3DSeries::Mesh style;
    bool smooth;
    int itemCount;
    float curveDivider;
};

#endif // PLOTTER_H
