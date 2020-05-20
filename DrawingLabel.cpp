#include "DrawingLabel.h"

#include <QMouseEvent>
#include <QPainter>

DrawingLabel::DrawingLabel(QWidget *parent) : QLabel(parent),
    m_line(),
    m_gridVisible(false),
    m_gridSize()
{
}

void DrawingLabel::setGridVisible(bool visible)
{
    m_gridVisible = visible;
    update();
}

void DrawingLabel::setGridSize(QSize size)
{
    m_gridSize = size;
    update();
}

void DrawingLabel::paintEvent(QPaintEvent *ev)
{
    QLabel::paintEvent(ev);

    QPainter painter(this);

    if (m_gridVisible && !m_gridSize.isNull()) {
        for (int x = 0, w = width(), dw = m_gridSize.width(); x < w; x += dw) {
            QPen pen(Qt::green);
            pen.setStyle(Qt::DashLine);
            painter.setPen(pen);

            painter.drawLine(x, 0, x, height());
        }

        for (int y = 0, h = height(), dh = m_gridSize.height(); y < h; y += dh) {
            QPen pen(Qt::green);
            pen.setStyle(Qt::DashLine);
            painter.setPen(pen);

            painter.drawLine(0, y, width(), y);
        }
    }

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
