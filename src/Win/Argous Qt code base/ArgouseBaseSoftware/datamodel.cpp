#include "datamodel.h"
#include "ui_datamodel.h"

DataModel::DataModel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataModel)
{
    ui->setupUi(this);
    generate3DWidget();
}

void DataModel::generate3DWidget(){
    Q3DScatter* graph = new Q3DScatter();
    QWidget* container = QWidget::createWindowContainer(graph);

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.5));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    QHBoxLayout* hLayout = new QHBoxLayout(this);
    QVBoxLayout* vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);
    this->setWindowTitle(QStringLiteral("Le fancy 3D model"));

    QComboBox* themeList = new QComboBox(this);
    themeList->addItem(QStringLiteral("Qt"));
    themeList->addItem(QStringLiteral("Primary Colors"));
    themeList->addItem(QStringLiteral("Digia"));
    themeList->addItem(QStringLiteral("Stone Moss"));
    themeList->addItem(QStringLiteral("Army Blue"));
    themeList->addItem(QStringLiteral("Retro"));
    themeList->addItem(QStringLiteral("Ebony"));
    themeList->addItem(QStringLiteral("Isabelle"));
    themeList->setCurrentIndex(6);

    QPushButton* labelButton = new QPushButton(this);
    labelButton->setText(QStringLiteral("Change label style"));

    QCheckBox* smoothCheckBox = new QCheckBox(this);
    smoothCheckBox->setText(QStringLiteral("Smooth dots"));
    smoothCheckBox->setChecked(true);

    QComboBox* itemStyleList = new QComboBox(this);
    itemStyleList->addItem(QStringLiteral("Sphere"), int(QAbstract3DSeries::MeshSphere));
    itemStyleList->addItem(QStringLiteral("Cube"), int(QAbstract3DSeries::MeshCube));
    itemStyleList->addItem(QStringLiteral("Minimal"), int(QAbstract3DSeries::MeshMinimal));
    itemStyleList->addItem(QStringLiteral("Point"), int(QAbstract3DSeries::MeshPoint));
    itemStyleList->setCurrentIndex(0);

    QPushButton* cameraButton = new QPushButton(this);
    cameraButton->setText(QStringLiteral("Change camera preset"));

    QPushButton* itemCountButton = new QPushButton(this);
    itemCountButton->setText(QStringLiteral("Toggle item count"));

    QCheckBox* backgroundCheckBox = new QCheckBox(this);
    backgroundCheckBox->setText(QStringLiteral("Show background"));
    backgroundCheckBox->setChecked(true);

    QCheckBox* gridCheckBox = new QCheckBox(this);
    gridCheckBox->setText(QStringLiteral("Show grid"));
    gridCheckBox->setChecked(true);

    QComboBox* shadowQuality = new QComboBox(this);
    shadowQuality->addItem(QStringLiteral("None"));
    shadowQuality->addItem(QStringLiteral("Low"));
    shadowQuality->addItem(QStringLiteral("Medium"));
    shadowQuality->addItem(QStringLiteral("High"));
    shadowQuality->addItem(QStringLiteral("Low Soft"));
    shadowQuality->addItem(QStringLiteral("Medium Soft"));
    shadowQuality->addItem(QStringLiteral("High Soft"));
    shadowQuality->setCurrentIndex(4);

    QFontComboBox* fontList = new QFontComboBox(this);
    fontList->setCurrentFont(QFont("Arial"));

    vLayout->addWidget(labelButton, 0, Qt::AlignTop);
    vLayout->addWidget(cameraButton, 0, Qt::AlignTop);
    vLayout->addWidget(itemCountButton, 0, Qt::AlignTop);
    vLayout->addWidget(backgroundCheckBox);
    vLayout->addWidget(gridCheckBox);
    vLayout->addWidget(smoothCheckBox, 0, Qt::AlignTop);
    vLayout->addWidget(new QLabel(QStringLiteral("Change dot style")));
    vLayout->addWidget(itemStyleList);
    vLayout->addWidget(new QLabel(QStringLiteral("Change theme")));
    vLayout->addWidget(themeList);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
    vLayout->addWidget(shadowQuality);
    vLayout->addWidget(new QLabel(QStringLiteral("Change font")));
    vLayout->addWidget(fontList, 1, Qt::AlignTop);

    Plotter* modifier = new  Plotter(graph);

    QObject::connect(cameraButton, &QPushButton::clicked, modifier,
        & Plotter::changePresetCamera);
    QObject::connect(labelButton, &QPushButton::clicked, modifier,
        & Plotter::changeLabelStyle);
    QObject::connect(itemCountButton, &QPushButton::clicked, modifier,
        & Plotter::toggleItemCount);

    QObject::connect(backgroundCheckBox, &QCheckBox::stateChanged, modifier,
        & Plotter::setBackgroundEnabled);
    QObject::connect(gridCheckBox, &QCheckBox::stateChanged, modifier,
        & Plotter::setGridEnabled);
    QObject::connect(smoothCheckBox, &QCheckBox::stateChanged, modifier,
        & Plotter::setSmoothDots);

    QObject::connect(modifier, & Plotter::backgroundEnabledChanged,
        backgroundCheckBox, &QCheckBox::setChecked);
    QObject::connect(modifier, & Plotter::gridEnabledChanged,
        gridCheckBox, &QCheckBox::setChecked);
    QObject::connect(itemStyleList, SIGNAL(currentIndexChanged(int)), modifier,
        SLOT(changeStyle(int)));

    QObject::connect(themeList, SIGNAL(currentIndexChanged(int)), modifier,
        SLOT(changeTheme(int)));

    QObject::connect(shadowQuality, SIGNAL(currentIndexChanged(int)), modifier,
        SLOT(changeShadowQuality(int)));

    QObject::connect(modifier, & Plotter::shadowQualityChanged, shadowQuality,
        &QComboBox::setCurrentIndex);
    QObject::connect(graph, &Q3DScatter::shadowQualityChanged, modifier,
        & Plotter::shadowQualityUpdatedByVisual);

    QObject::connect(fontList, &QFontComboBox::currentFontChanged, modifier,
        & Plotter::changeFont);

    QObject::connect(modifier, & Plotter::fontChanged, fontList,
        &QFontComboBox::setCurrentFont);
}

DataModel::~DataModel()
{
    delete ui;
}


