#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QScrollArea;
class QSlider;
class QSpinBox;
class QDoubleSpinBox;
class DrawingLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void setScale(int factor);
    void setLength(double dx, double dy);

private:
    void makeMenu();
    void resetScale();
    void updateExif(const QString &fileName);

    QScrollArea *m_area;
    DrawingLabel *m_imageLabel;
    QSlider *m_zoomSlider;
    QDoubleSpinBox *m_focalLengthSpinBox;
    QDoubleSpinBox *m_imageWidthSpinBox;
    QDoubleSpinBox *m_imageHeightSpinBox;
    QDoubleSpinBox *m_sensorWidthSpinBox;
    QDoubleSpinBox *m_sensorHeightSpinBox;

    double m_scaleFactor = 1.0;
};

#endif // MAINWINDOW_H
