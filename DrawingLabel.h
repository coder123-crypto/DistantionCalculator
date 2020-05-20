#ifndef DRAWINGLABEL_H
#define DRAWINGLABEL_H

#include <QLabel>
#include <QSize>

class DrawingLabel : public QLabel
{
    Q_OBJECT

public:
    DrawingLabel(QWidget *parent);

public slots:
    void setGridVisible(bool visible);
    void setGridSize(QSize size);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;

signals:
    void lengthChanged(QLineF length);

private:
    QLineF m_line;
    bool m_gridVisible;
    QSize m_gridSize;
};

#endif // DRAWINGLABEL_H
