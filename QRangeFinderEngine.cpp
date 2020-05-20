#include "QRangeFinderEngine.h"

#include <QtMath>

#include <cmath>

QRangeFinderEngine::QRangeFinderEngine(QObject *parent) : QObject(parent),
    m_focalLength(std::numeric_limits<double>::quiet_NaN()),
    m_planeResolutionX(std::numeric_limits<double>::quiet_NaN()),
    m_planeResolutionY(std::numeric_limits<double>::quiet_NaN()),
    m_objectSize(std::numeric_limits<double>::quiet_NaN()),
    m_objectRuler(QLineF())
{
    connect(this, &QRangeFinderEngine::focalLengthChanged, this, &QRangeFinderEngine::updateDistantion);
    connect(this, &QRangeFinderEngine::planeResolutionXChanged, this, &QRangeFinderEngine::updateDistantion);
    connect(this, &QRangeFinderEngine::planeResolutionYChanged, this, &QRangeFinderEngine::updateDistantion);
    connect(this, &QRangeFinderEngine::objectSizeChanged, this, &QRangeFinderEngine::updateDistantion);
}

double QRangeFinderEngine::pixelsPerMeterX(double distantion) const
{
    if (!std::isnan(focalLength()) && !std::isnan(planeResolutionX())) {
        const double resolution = planeResolutionX() * 1000.0;
        return qAbs(focalLength() * 1.0 * resolution / (focalLength() - distantion));
    }
    else
        return 0.0;
}

double QRangeFinderEngine::pixelsPerMeterY(double distantion) const
{
    if (!std::isnan(focalLength()) && !std::isnan(planeResolutionY())) {
        const double resolution = planeResolutionY() * 1000.0;
        return qAbs(focalLength() * 1.0 * resolution / (focalLength() - distantion));
    }
    else
        return 0.0;
}

void QRangeFinderEngine::setFocalLength(const double focalLength)
{
    m_focalLength = focalLength;
    emit focalLengthChanged(this->focalLength());
}

void QRangeFinderEngine::setPlaneResolutionX(const double resolution, const PlaneResolutionUnit unit)
{
    switch (unit) {
    case PlaneResolutionUnit::Centimeter:
        m_planeResolutionX = resolution * 0.1;
        break;

    default:
        m_planeResolutionX = resolution;
        break;
    }

    emit planeResolutionXChanged(planeResolutionX());
}

void QRangeFinderEngine::setPlaneResolutionY(const double resolution, const PlaneResolutionUnit unit)
{
    switch (unit) {
    case PlaneResolutionUnit::Centimeter:
        m_planeResolutionY = resolution * 0.1;
        break;

    default:
        m_planeResolutionY = resolution;
        break;
    }

    emit planeResolutionYChanged(planeResolutionY());
}

void QRangeFinderEngine::setObjectSize(const double size)
{
    m_objectSize = size;
    emit objectSizeChanged(objectSize());
}

void QRangeFinderEngine::setObjectRuler(const QLineF ruler)
{
    m_objectRuler = ruler;
    updateDistantion();
}

double QRangeFinderEngine::focalLength() const
{
    return m_focalLength;
}

double QRangeFinderEngine::planeResolutionX() const
{
    return m_planeResolutionX;
}

double QRangeFinderEngine::planeResolutionY() const
{
    return m_planeResolutionY;
}

double QRangeFinderEngine::objectSize() const
{
    return m_objectSize;
}

const QLineF &QRangeFinderEngine::objectRuler() const
{
    return m_objectRuler;
}

void QRangeFinderEngine::updateDistantion()
{
    if (!std::isnan(focalLength()) && !std::isnan(planeResolutionX()) && !std::isnan(planeResolutionX()) && !std::isnan(objectSize()) && !objectRuler().isNull()) {
        const double dx = objectRuler().dx();
        const double dy = objectRuler().dy();
        const double resolutionX = planeResolutionX() * 1000.0;
        const double resolutionY = planeResolutionY() * 1000.0;

        const double sizeOnSensor = qSqrt(qPow(dx / resolutionX, 2.0) + qPow(dy / resolutionY, 2.0));
        const double distantion = focalLength() * (objectSize() + sizeOnSensor) / sizeOnSensor;

        emit objectDistantionChanged(distantion);
    }
    else
        emit objectDistantionChanged(0.0);
}
