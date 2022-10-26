/****************************************************
 * Description: Plotter plots the 3D visualisation using the temp .PLY data file created when importing data.
 * It is embeded within the datamodel class.
 *
 * This 3D visualisation has a series of customisation options that may be called by the user. These include:
 *      changeStyle
 *      setSmoothDots
 *      changeTheme
 *      changePresetCamera
 *      changeLabelStyle
 *      changeFont
 *      changeShadowQuality/shadowQualityUpdatedByVisual
 *      setBackgroundEnabled
 *      setGridEnabled
 *
 * Author/s: Monique Kuhn
****************************************************/

#include "plotter.h"

/*Can be edited to vary the outcome of the 3D model. Ensure that number of items will be able to hold all data points*/
const int numberOfItems = 3600;
const int lowerNumberOfItems = 5000;

const float curveDivider = 3.0f;
const float lowerCurveDivider = 0.75f;


Plotter::Plotter(Q3DScatter* scatter)
    : graph(scatter),
        fontSize(40.0f),
        style(QAbstract3DSeries::MeshSphere),
        smooth(true),
        itemCount(lowerNumberOfItems),
        curveDivider(lowerCurveDivider)
{
    /*Sets the initial theme, font (type & size), shadow quality, and camera preset for the graph*/
    graph->activeTheme()->setType(Q3DTheme::ThemeEbony);
    QFont font = graph->activeTheme()->font();
    font.setPointSize(fontSize);
    graph->activeTheme()->setFont(font);
    graph->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftLow);
    graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);
    //graph->isOrthoProjection();
    graph->setOrthoProjection(true);
    graph->setAspectRatio(1);
    graph->setHorizontalAspectRatio(1);

    /*Configures X, Y, Z axis units. These are hard coded to match the format the distance data is delivered in*/
    graph->axisX()->setTitle("X (cm)");
    graph->axisX()->setTitleVisible(true);

    graph->axisY()->setTitle("Z (cm)");
    graph->axisY()->setTitleVisible(true);

    graph->axisZ()->setTitle("Y (cm)");
    graph->axisZ()->setTitleVisible(true);

    /*Initialises 3D Scatter Proxy and Series*/
    QScatterDataProxy* proxy = new QScatterDataProxy;
    QScatter3DSeries* series = new QScatter3DSeries(proxy);

    /*Formats series X, Y, Z labels and mesh before adding to the graph*/
    series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @zTitle: @zLabel @yTitle: @yLabel"));
    series->setMeshSmooth(smooth);
    graph->addSeries(series);
}

Plotter::~Plotter()
{
    delete graph;
}


/*addData() extracts the data from the temp .PLY file (populated on import) and loads it into the data array*/
void Plotter::addData()
{
    QScatterDataArray* dataArray = new QScatterDataArray;
    dataArray->resize(itemCount);
    QScatterDataItem* ptrToDataArray = &dataArray->first();

    /*References the temporary .PLY file created on import in homepage.cpp*/
    QString filename = "../ArgouseBaseSoftware/appdata/as7-map.ply";
    QFile file(filename);
    QVector<QString> importedPLYData;
    QMessageBox msg;

    /*Reads data from temporary .PLY file line by line*/
    if(filename.isEmpty()){
        msg.setText("Mapping data not found! Has the data been successfully imported?");
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

    /*Loads data from populated array*/
    for (int i = 0; i < importedPLYData.length(); i++){
        ptrToDataArray->setPosition(PLYPoint(importedPLYData[i]));
        ptrToDataArray++;
    }

    graph->seriesList().at(0)->dataProxy()->resetArray(dataArray);
}

/*PLYPoint(QString line) reads a line (formatted <X> <Y> <Z>) and transforms it into a QVector3D point*/
QVector3D Plotter::PLYPoint(QString line){
    QString stX = "";
    QString stY = "";
    QString stZ = "";

    int pos = 0;

    for(QChar& c : line) {
        if(c == ' '){pos += 1;}
        else{
           if(pos == 0){stX = stX + c;}
           else if(pos == 1){stZ = stZ + c;}
           else if(pos == 2){stY = stY + c;}
        }
    }

    float fX = stX.toFloat();
    float fY = stY.toFloat();
    float fZ = stZ.toFloat();

    return QVector3D(fX, fY, fZ);
}

/*changeStyle() changes the style of the graph*/
void Plotter::changeStyle(int _style)
{
    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
    if (comboBox) {
        style = QAbstract3DSeries::Mesh(comboBox->itemData(_style).toInt());
        if (graph->seriesList().size())
            graph->seriesList().at(0)->setMesh(style);
    }
}

/*setSmoothDots() sets the smoothness of the dots*/
void Plotter::setSmoothDots(int _smooth)
{
    smooth = bool(_smooth);
    QScatter3DSeries* series = graph->seriesList().at(0);
    series->setMeshSmooth(smooth);
}

/*changeTheme() changes the graph theme based on checkboxes*/
void Plotter::changeTheme(int _theme)
{
    Q3DTheme* currentTheme = graph->activeTheme();
    currentTheme->setType(Q3DTheme::Theme(_theme));
    emit backgroundEnabledChanged(currentTheme->isBackgroundEnabled());
    emit gridEnabledChanged(currentTheme->isGridEnabled());
    emit fontChanged(currentTheme->font());
}

/*changePresetCamera() changes the angle that the 3D visualisation is viewed from*/
void Plotter::changePresetCamera()
{
    static int preset = Q3DCamera::CameraPresetFrontLow;

    graph->scene()->activeCamera()->setCameraPreset((Q3DCamera::CameraPreset)preset);

    if (++preset > Q3DCamera::CameraPresetDirectlyBelow)
    preset = Q3DCamera::CameraPresetFrontLow;
}

/*changeLabelStyle() changes the styling of the label*/
void Plotter::changeLabelStyle()
{
    graph->activeTheme()->setLabelBackgroundEnabled(!graph->activeTheme()->isLabelBackgroundEnabled());
}

/*changeFont() updates the graph with the selected font*/
void Plotter::changeFont(const QFont& _font)
{
    QFont newFont = _font;
    newFont.setPointSizeF(fontSize);
    graph->activeTheme()->setFont(newFont);
}

/*shadowQualityUpdatedByVisual() updates the shadow quality of the graph based on checkbox*/
void Plotter::shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality _shadowQ)
{
    int shadowQ = int(_shadowQ);
    emit shadowQualityChanged(shadowQ);
}

/*shadowQualityUpdatedByVisual() updates the shadow quality of the graph*/
void Plotter::changeShadowQuality(int _quality)
{
    QAbstract3DGraph::ShadowQuality shadowQ = QAbstract3DGraph::ShadowQuality(_quality);
    graph->setShadowQuality(shadowQ);
}

/*setBackgroundEnabled() sets the background*/
void Plotter::setBackgroundEnabled(int _enabled)
{
    graph->activeTheme()->setBackgroundEnabled((bool) _enabled);
}

/*setBackgroundEnabled() sets the background*/
void Plotter::setGridEnabled(int _enabled)
{
    graph->activeTheme()->setGridEnabled((bool) _enabled);
}

/*toggleItemCount() determines the curver divider and item count prior to adding data.
 * This is called by the datamodel.cpp when the widget is generated.*/
void Plotter::toggleItemCount()
{
    if (itemCount == numberOfItems) {
        itemCount = lowerNumberOfItems;
        curveDivider = lowerCurveDivider;
    }
    else {
        itemCount = numberOfItems;
        curveDivider = curveDivider;
    }
    graph->seriesList().at(0)->dataProxy()->resetArray(0);
    addData();
}

/*randVector() is a useful tool for debugging. It generates a random 3D vector*/
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
