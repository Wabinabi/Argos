#include "plotter.h"

//#define RANDOM_SCATTER // Uncomment this to switch to random scatter
const int numberOfItems = 3600;
const float curveDivider = 3.0f;
const int lowerNumberOfItems = 900;
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

//#ifdef RANDOM_SCATTER


//#else
    float limit = qSqrt(m_itemCount) / 2.0f;
    for (float i = -limit; i < limit; i++) {
        for (float j = -limit; j < limit; j++) {
            ptrToDataArray->setPosition(QVector3D(i + 0.5f,
                qCos(qDegreesToRadians((i * j) / m_curveDivider)),
                j + 0.5f));
            ptrToDataArray++;
        }
    }
//#endif

    m_graph->seriesList().at(0)->dataProxy()->resetArray(dataArray);
}

//void Plotter::closeEvent (QCloseEvent *event)
//{
//        event->ignore();
//}

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
