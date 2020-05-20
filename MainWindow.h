#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QScrollArea;
class QSlider;
class QSpinBox;
class QDoubleSpinBox;
class DrawingLabel;
class QRangeFinderEngine;
class QCheckBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void setScale(int factor);
    void updateGrid();

private:
    void makeMenu();
    void resetScale();
    bool updateExif(const QString &fileName);

    QScrollArea *m_area;
    DrawingLabel *m_imageLabel;
    QSlider *m_zoomSlider;
    QDoubleSpinBox *m_focalLengthSpinBox;
    QDoubleSpinBox *m_planeResolutionXSpinBox;
    QDoubleSpinBox *m_planeResolutionYSpinBox;
    QDoubleSpinBox *m_objectSizeSpinBox;
    QDoubleSpinBox *m_distantionSpinBox;
    QCheckBox *m_gridVisibleCheckBox;
    QDoubleSpinBox *m_gridSizeSpinBox;
    QDoubleSpinBox *m_gridDistantionSpinBox;

    double m_scaleFactor = 1.0;
    QRangeFinderEngine *m_engine;
};

#endif // MAINWINDOW_H
