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
#include <QtMath>

#include <DrawingLabel.h>
#include <ExifHelpers.h>

void adjustScrollBar(QScrollBar *scrollBar, const double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    m_area(new QScrollArea(this)),
    m_imageLabel(new DrawingLabel(this)),
    m_zoomSlider(new QSlider(Qt::Vertical, this)),
    m_focalLengthSpinBox(new QDoubleSpinBox(this)),
    m_imageWidthSpinBox(new QDoubleSpinBox(this)),
    m_imageHeightSpinBox(new QDoubleSpinBox(this)),
    m_sensorWidthSpinBox(new QDoubleSpinBox(this)),
    m_sensorHeightSpinBox(new QDoubleSpinBox(this))
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
        spinBox->setButtonSymbols(QSpinBox::NoButtons);
        spinBox->setRange(0, 1000000);
        spinBox->setAlignment(Qt::AlignRight);

        if (spinBox != m_sensorWidthSpinBox)
            spinBox->setReadOnly(true);
    }

    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    QGridLayout *layout = new QGridLayout(centralWidget());
    layout->setSpacing(6);

    layout->addWidget(new QLabel(tr("Фокусное расстояние, мм:"), this), 0, 1);
    layout->addWidget(m_focalLengthSpinBox, 0, 2);

    layout->addWidget(new QLabel(tr("Разрешение по X, ppm:"), this), 0, 3);
    layout->addWidget(m_imageWidthSpinBox, 0, 4);
    layout->addWidget(new QLabel(tr("Разрешение по Y, ppm:"), this), 1, 3);
    layout->addWidget(m_imageHeightSpinBox, 1, 4);

    layout->addWidget(new QLabel(tr("Длина объекта, м:"), this), 0, 5);
    layout->addWidget(m_sensorWidthSpinBox, 0, 6);
    layout->addWidget(new QLabel(tr("Расстояние, м:"), this), 1, 5);
    layout->addWidget(m_sensorHeightSpinBox, 1, 6);

    layout->addItem(spacer, 0, 7);

    layout->addWidget(m_zoomSlider, 2, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(m_area, 2, 1, 1, 7);

    connect(m_imageLabel, &DrawingLabel::lengthChanged, this, &MainWindow::setLength);
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
        const QString unit = parseString(ed, EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT);
        const double focalLength = parseFocalLength(ed);
        const double planeXResolution = parseDouble(ed, EXIF_TAG_FOCAL_PLANE_X_RESOLUTION);
        const double planeYResolution = parseDouble(ed, EXIF_TAG_FOCAL_PLANE_X_RESOLUTION);

        if (unit.isEmpty()) {
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

        double multiple = 1.0;
        if (unit == "Centimeter")
            multiple = 0.1;

        m_focalLengthSpinBox->setValue(focalLength);
        m_imageWidthSpinBox->setValue(planeXResolution * multiple);
        m_imageHeightSpinBox->setValue(planeYResolution * multiple);

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
}

void MainWindow::setLength(double dx, double dy)
{
    dx /=  m_scaleFactor;
    dy /=  m_scaleFactor;

    const double focalLength = m_focalLengthSpinBox->value();
    const double xResolution = m_imageWidthSpinBox->value();
    const double yResolution = m_imageHeightSpinBox->value();
    const double objectSize = m_sensorWidthSpinBox->value() * 1000.0;

    const double sizeOnSensor = qSqrt(qPow(dx / xResolution, 2) + qPow(dy / yResolution, 2));
    const double distantion = focalLength * (objectSize + sizeOnSensor) / sizeOnSensor;

    m_sensorHeightSpinBox->setValue(distantion / 1000.0);
}

