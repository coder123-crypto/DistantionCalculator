#ifndef QRANGEFINDERENGINE_H
#define QRANGEFINDERENGINE_H

#include <QObject>
#include <QLineF>

#include <PlaneResolutionUnit.h>

class QRangeFinderEngine : public QObject
{
    Q_OBJECT

public:
    explicit QRangeFinderEngine(QObject *parent = nullptr);

signals:
    void focalLengthChanged(double focalLength);
    void planeResolutionXChanged(double resolution);
    void planeResolutionYChanged(double resolution);
    void objectDistantionChanged(double distantion);
    void objectSizeChanged(double size);

public:
    void setFocalLength(const double focalLength);
    void setPlaneResolutionX(const double resolution, const PlaneResolutionUnit unit);
    void setPlaneResolutionY(const double resolution, const PlaneResolutionUnit unit);
    void setObjectSize(const double size);
    void setObjectRuler(const QLineF ruler);

    double focalLength() const;
    double planeResolutionX() const;
    double planeResolutionY() const;
    double objectSize() const;
    const QLineF &objectRuler() const;

private slots:
    void updateDistantion();

private:
    double m_focalLength;
    double m_planeResolutionX;
    double m_planeResolutionY;
    double m_objectSize;
    QLineF m_objectRuler;
};

#endif // QRANGEFINDERENGINE_H
