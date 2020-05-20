#include "DrawingLabel.h"

#include <QMouseEvent>
#include <QPainter>

DrawingLabel::DrawingLabel(QWidget *parent) : QLabel(parent),
    m_line()
{
}

void DrawingLabel::paintEvent(QPaintEvent *ev)
{
    QLabel::paintEvent(ev);

    QPainter painter(this);

    if (m_line.length() > 1) {
        painter.setPen(QPen(QColor(255, 0, 0)));
        painter.drawLine(m_line);
    }

    update();
}

void DrawingLabel::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        m_line.setP1(ev->pos());
        m_line.setP2(ev->pos());
    }
}

void DrawingLabel::mouseMoveEvent(QMouseEvent *ev)
{
    m_line.setP2(ev->pos());

    emit lengthChanged(m_line);
}
