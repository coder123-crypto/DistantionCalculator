#include "MainWindow.h"

#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QScrollArea>
#include <QLabel>
#include <QImageReader>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScrollBar>
#include <QGridLayout>
#include <QSlider>
#include <QSpinBox>
#include <QSpacerItem>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QHBoxLayout>

#include <DrawingLabel.h>
#include <ExifHelpers.h>
#include <QRangeFinderEngine.h>

void adjustScrollBar(QScrollBar *scrollBar, const double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    m_area(new QScrollArea(this)),
    m_imageLabel(new DrawingLabel(this)),
    m_zoomSlider(new QSlider(Qt::Vertical, this)),
    m_focalLengthSpinBox(new QDoubleSpinBox(this)),
    m_planeResolutionXSpinBox(new QDoubleSpinBox(this)),
    m_planeResolutionYSpinBox(new QDoubleSpinBox(this)),
    m_objectSizeSpinBox(new QDoubleSpinBox(this)),
    m_distantionSpinBox(new QDoubleSpinBox(this)),
    m_gridVisibleCheckBox(new QCheckBox(tr("Отображать сетку"), this)),
    m_gridSizeSpinBox(new QDoubleSpinBox(this)),
    m_gridDistantionSpinBox(new QDoubleSpinBox(this)),
    m_engine(new QRangeFinderEngine(this))
{
    setCentralWidget(new QWidget(this));
    makeMenu();

    m_imageLabel->setBackgroundRole(QPalette::Base);
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageLabel->setScaledContents(true);

    m_area->setBackgroundRole(QPalette::Dark);
    m_area->setWidget(m_imageLabel);
    m_area->setVisible(false);

    m_zoomSlider->setRange(10, 100);
    m_zoomSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_zoomSlider->setTickInterval(10);
    m_zoomSlider->setTickPosition(QSlider::TicksLeft);
    m_zoomSlider->setValue(100);
    m_zoomSlider->setTickInterval(10);
    connect(m_zoomSlider, &QSlider::valueChanged, this, &MainWindow::setScale);

    foreach (QDoubleSpinBox *spinBox, findChildren<QDoubleSpinBox*>()) {
        spinBox->setDecimals(3);
        spinBox->setRange(0, 1000000);
        spinBox->setAlignment(Qt::AlignRight);

        if (spinBox != m_objectSizeSpinBox && spinBox != m_gridSizeSpinBox && spinBox != m_gridDistantionSpinBox)
            spinBox->setReadOnly(true);

        if (spinBox != m_gridSizeSpinBox && spinBox != m_gridDistantionSpinBox)
            spinBox->setButtonSymbols(QSpinBox::NoButtons);
    }

    QGridLayout *layout = new QGridLayout(centralWidget());
    layout->setSpacing(6);

    // Создание шапки
    layout->addWidget(new QLabel(tr("Фокусное расстояние, мм:"), this), 0, 1);
    layout->addWidget(m_focalLengthSpinBox, 0, 2);

    layout->addWidget(new QLabel(tr("Разрешение по X, ppm:"), this), 0, 3);
    layout->addWidget(m_planeResolutionXSpinBox, 0, 4);
    layout->addWidget(new QLabel(tr("Разрешение по Y, ppm:"), this), 1, 3);
    layout->addWidget(m_planeResolutionYSpinBox, 1, 4);

    layout->addWidget(new QLabel(tr("Длина объекта, м:"), this), 0, 5);
    layout->addWidget(m_objectSizeSpinBox, 0, 6);
    layout->addWidget(new QLabel(tr("Расстояние, м:"), this), 1, 5);
    layout->addWidget(m_distantionSpinBox, 1, 6);

    layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 7);

    layout->addWidget(m_zoomSlider, 2, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(m_area, 2, 1, 1, 7);

    // Создание управления сеткой
    QHBoxLayout *gridLayout = new QHBoxLayout();

    gridLayout->addWidget(m_gridVisibleCheckBox);
    gridLayout->addWidget(new QLabel(tr("размером, м:"), this));
    gridLayout->addWidget(m_gridSizeSpinBox);
    gridLayout->addWidget(new QLabel(tr("на расстоянии, м:"), this));
    gridLayout->addWidget(m_gridDistantionSpinBox);
    gridLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    layout->addLayout(gridLayout, 3, 1, 1, 7);

    // Сигналы
    connect(m_imageLabel, &DrawingLabel::lengthChanged, m_engine, [=](QLineF d) {
        d.setLength(d.length() / m_scaleFactor);
        m_engine->setObjectRuler(d);
    });

    connect(m_objectSizeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double d) { m_engine->setObjectSize(d); });

    connect(m_gridVisibleCheckBox, &QCheckBox::stateChanged, [=](int s) { m_imageLabel->setGridVisible(s == Qt::Checked); });
    connect(m_gridSizeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double) { updateGrid(); });
    connect(m_gridDistantionSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double) { updateGrid(); });

    connect(m_engine, &QRangeFinderEngine::focalLengthChanged, [=](double d){ m_focalLengthSpinBox->setValue(d * 1000.0); });
    connect(m_engine, &QRangeFinderEngine::planeResolutionXChanged, m_planeResolutionXSpinBox, &QDoubleSpinBox::setValue);
    connect(m_engine, &QRangeFinderEngine::planeResolutionYChanged, m_planeResolutionYSpinBox, &QDoubleSpinBox::setValue);
    connect(m_engine, &QRangeFinderEngine::objectDistantionChanged, m_distantionSpinBox, &QDoubleSpinBox::setValue);
}

MainWindow::~MainWindow()
{
}

void MainWindow::openFile()
{
    static QString selectedPath = QDir::homePath();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть фотографию"), selectedPath, tr("Файлы фотографий (*.jpg *.jpeg)"));
    if (QFile::exists(fileName)) {
        selectedPath = QFileInfo(fileName).path();

        if (updateExif(fileName)) {
            QImage image(fileName);
            m_imageLabel->setPixmap(QPixmap::fromImage(image));
            m_area->setVisible(true);
            m_imageLabel->adjustSize();

            resetScale();
        }
    }
}

void MainWindow::makeMenu()
{
    QMenu *fileMenu = new QMenu(tr("Файл"), this);
    fileMenu->addAction(tr("Открыть файл"), this, &MainWindow::openFile);

    setMenuBar(new QMenuBar(this));
    menuBar()->addMenu(fileMenu);
}

void MainWindow::resetScale()
{
    m_zoomSlider->setValue(100);
}

bool MainWindow::updateExif(const QString& fileName)
{
    ExifData *ed = exif_data_new_from_file(fileName.toLocal8Bit().data());

    if (ed != nullptr) {
        const QString resolutionUnit = parseString(ed, EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT);
        const double focalLength = parseFocalLength(ed);
        const double planeXResolution = parseDouble(ed, EXIF_TAG_FOCAL_PLANE_X_RESOLUTION);
        const double planeYResolution = parseDouble(ed, EXIF_TAG_FOCAL_PLANE_X_RESOLUTION);

        if (resolutionUnit.isEmpty()) {
            QMessageBox::warning(this, tr("Ошибка"), tr("В файле отсутствуют EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT"));
            return false;
        }

        if (std::isnan(focalLength)) {
            QMessageBox::warning(this, tr("Ошибка"), tr("В файле отсутствуют EXIF_TAG_FOCAL_LENGTH"));
            return false;
        }

        if (std::isnan(planeXResolution)) {
            QMessageBox::warning(this, tr("Ошибка"), tr("В файле отсутствуют EXIF_TAG_FOCAL_PLANE_X_RESOLUTION"));
            return false;
        }

        if (std::isnan(planeYResolution)) {
            QMessageBox::warning(this, tr("Ошибка"), tr("В файле отсутствуют EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION"));
            return false;
        }

        PlaneResolutionUnit unit = PlaneResolutionUnit::Centimeter;
        if (resolutionUnit == "Centimeter")
            unit = PlaneResolutionUnit::Centimeter;

        m_engine->setFocalLength(focalLength / 1000.0);
        m_engine->setPlaneResolutionX(planeXResolution, unit);
        m_engine->setPlaneResolutionY(planeYResolution, unit);

        delete ed;
        return true;
    }
    else {
        QMessageBox::warning(this, tr("Ошибка"), tr("В файле отсутствуют exif данные"));
        return false;
    }
}

void MainWindow::setScale(int factor)
{
    m_scaleFactor = factor / 100.0;
    m_imageLabel->resize(m_scaleFactor * m_imageLabel->pixmap()->size());

    adjustScrollBar(m_area->horizontalScrollBar(), factor);
    adjustScrollBar(m_area->verticalScrollBar(), factor);

    updateGrid();
}

void MainWindow::updateGrid()
{
    int width = qRound(m_gridSizeSpinBox->value() * m_engine->pixelsPerMeterX(m_gridDistantionSpinBox->value()) * m_scaleFactor);
    int height = qRound(m_gridSizeSpinBox->value() * m_engine->pixelsPerMeterY(m_gridDistantionSpinBox->value()) * m_scaleFactor);

    m_imageLabel->setGridSize(QSize(width, height));
}
