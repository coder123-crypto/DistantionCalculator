#ifndef DRAWINGLABEL_H
#define DRAWINGLABEL_H

#include <QLabel>

class DrawingLabel : public QLabel
{
    Q_OBJECT

public:
    DrawingLabel(QWidget *parent);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;

signals:
    void lengthChanged(double dx, double dy);

private:
    QLineF m_line;
};

#endif // DRAWINGLABEL_H
