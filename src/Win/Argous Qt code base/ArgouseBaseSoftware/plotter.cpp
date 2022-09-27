#include "plotter.h"

const int numberOfItems = 3600;
const float curveDivider = 3.0f;
const int lowerNumberOfItems = 5000;
const float lowerCurveDivider = 0.75f;


Plotter::Plotter(Q3DScatter* scatter)
    : m_graph(scatter),
        m_fontSize(40.0f),
        m_style(QAbstract3DSeries::MeshSphere),
        m_smooth(true),
        m_itemCount(lowerNumberOfItems),
        m_curveDivider(lowerCurveDivider)
{

        m_graph->activeTheme()->setType(Q3DTheme::ThemeEbony);
        QFont font = m_graph->activeTheme()->font();
        font.setPointSize(m_fontSize);
        m_graph->activeTheme()->setFont(font);
        m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftLow);
        m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);


        QScatterDataProxy* proxy = new QScatterDataProxy;
        QScatter3DSeries* series = new QScatter3DSeries(proxy);
        series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
        series->setMeshSmooth(m_smooth);
        m_graph->addSeries(series);

        addData();
}

Plotter::~Plotter()
{
    delete m_graph;
}

void Plotter::addData()
{
    // Configure the axes according to the data
    m_graph->axisX()->setTitle("X");
    m_graph->axisY()->setTitle("Y");
    m_graph->axisZ()->setTitle("Z");

    QScatterDataArray* dataArray = new QScatterDataArray;
    dataArray->resize(m_itemCount);
    QScatterDataItem* ptrToDataArray = &dataArray->first();

    //Testing reading in a text PLY file (B1's cube)
    //Source file -> this will later link to the processed PLY file spat out by import action'
    QString filename = "test.txt";
    QFile file(filename);
    QVector<QString> importedPLYData;
    QMessageBox msg;

    if(filename.isEmpty()){
        msg.setText("Test file not present");
        msg.exec();
    }else{
        QString line;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream stream(&file);
                while (!stream.atEnd()){
                    line = stream.readLine();
                    importedPLYData.append(line);
                }
        msg.setText("Import complete!");
        msg.exec();

        file.close();
        }
    }

    //now load the data into the 3Dscatter plot
    for (float i = 0; i < importedPLYData.length(); i++){
        ptrToDataArray->setPosition(PLYPoint(importedPLYData[i]));
        ptrToDataArray++;
    }
    m_graph->seriesList().at(0)->dataProxy()->resetArray(dataArray);
}

//Processes a PLY line and spits out a single 3DVector point
QVector3D Plotter::PLYPoint(QString line){
    QString stX = "";
    QString stY = "";
    QString stZ = "";

    int pos = 0;

    for(QChar& c : line) {
        if(c == ' '){pos += 1;}
        else{
           if(pos == 0){stX = stX + c;}
           else if(pos == 1){stY = stY + c;}
           else if(pos == 2){stZ = stZ + c;}
        }
    }

    float fX = stX.toFloat();
    float fY = stY.toFloat();
    float fZ = stZ.toFloat();

    return QVector3D(fX, fY, fZ);
}


void Plotter::changeStyle(int style)
{
    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
    if (comboBox) {
        m_style = QAbstract3DSeries::Mesh(comboBox->itemData(style).toInt());
        if (m_graph->seriesList().size())
            m_graph->seriesList().at(0)->setMesh(m_style);
    }
}

void Plotter::setSmoothDots(int smooth)
{
    m_smooth = bool(smooth);
    QScatter3DSeries* series = m_graph->seriesList().at(0);
    series->setMeshSmooth(m_smooth);
}

void Plotter::changeTheme(int theme)
{
    Q3DTheme* currentTheme = m_graph->activeTheme();
    currentTheme->setType(Q3DTheme::Theme(theme));
    emit backgroundEnabledChanged(currentTheme->isBackgroundEnabled());
    emit gridEnabledChanged(currentTheme->isGridEnabled());
    emit fontChanged(currentTheme->font());
}

void Plotter::changePresetCamera()
{
    static int preset = Q3DCamera::CameraPresetFrontLow;

    m_graph->scene()->activeCamera()->setCameraPreset((Q3DCamera::CameraPreset)preset);

    if (++preset > Q3DCamera::CameraPresetDirectlyBelow)
        preset = Q3DCamera::CameraPresetFrontLow;
}

void Plotter::changeLabelStyle()
{
    m_graph->activeTheme()->setLabelBackgroundEnabled(!m_graph->activeTheme()->isLabelBackgroundEnabled());
}

void Plotter::changeFont(const QFont& font)
{
    QFont newFont = font;
    newFont.setPointSizeF(m_fontSize);
    m_graph->activeTheme()->setFont(newFont);
}

void Plotter::shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality sq)
{
    int quality = int(sq);
    emit shadowQualityChanged(quality); // connected to a checkbox in main.cpp
}

void Plotter::changeShadowQuality(int quality)
{
    QAbstract3DGraph::ShadowQuality sq = QAbstract3DGraph::ShadowQuality(quality);
    m_graph->setShadowQuality(sq);
}

void Plotter::setBackgroundEnabled(int enabled)
{
    m_graph->activeTheme()->setBackgroundEnabled((bool)enabled);
}

void Plotter::setGridEnabled(int enabled)
{
    m_graph->activeTheme()->setGridEnabled((bool)enabled);
}

void Plotter::toggleItemCount()
{
    if (m_itemCount == numberOfItems) {
        m_itemCount = lowerNumberOfItems;
        m_curveDivider = lowerCurveDivider;
    }
    else {
        m_itemCount = numberOfItems;
        m_curveDivider = curveDivider;
    }
    m_graph->seriesList().at(0)->dataProxy()->resetArray(0);
    addData();
}

QVector3D Plotter::randVector()
{
    return QVector3D(
        (float)(QRandomGenerator::global()->bounded(100)) / 2.0f -
        (float)(QRandomGenerator::global()->bounded(100)) / 2.0f,
        (float)(QRandomGenerator::global()->bounded(100)) / 100.0f -
        (float)(QRandomGenerator::global()->bounded(100)) / 100.0f,
        (float)(QRandomGenerator::global()->bounded(100)) / 2.0f -
        (float)(QRandomGenerator::global()->bounded(100)) / 2.0f);
}
