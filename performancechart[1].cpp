#include "performancechart.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QtMath>

PerformanceChart::PerformanceChart(QWidget *parent) : QWidget(parent)
{
    setMinimumHeight(210);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void PerformanceChart::setValues(const QVector<double> &accuracy,
                                 const QVector<double> &gameSense)
{
    accuracyValues = accuracy;
    gameSenseValues = gameSense;
    update();
}

void PerformanceChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor("#171827"));

    const int left = 42, right = 16, top = 18, bottom = 30;
    QRect area(left, top, width() - left - right, height() - top - bottom);

    p.setPen(QColor("#45465c"));
    for (int i = 0; i <= 4; ++i)
    {
        int y = area.bottom() - i * area.height() / 4;
        p.drawLine(area.left(), y, area.right(), y);
        p.setPen(QColor("#a5a6b8"));
        p.drawText(5, y + 4, QString::number(i * 25) + "%");
        p.setPen(QColor("#45465c"));
    }

    auto drawSeries = [&](const QVector<double> &values, const QColor &color)
    {
        if (values.isEmpty()) return;
        QPainterPath path;
        for (int i = 0; i < values.size(); ++i)
        {
            double x = area.left() + (values.size() == 1 ? area.width() / 2.0 :
                        i * area.width() / double(values.size() - 1));
            double v = qBound(0.0, values[i], 100.0);
            double y = area.bottom() - v * area.height() / 100.0;
            if (i == 0) path.moveTo(x, y); else path.lineTo(x, y);
        }
        p.setPen(QPen(color, 2.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPath(path);
        for (int i = 0; i < values.size(); ++i)
        {
            double x = area.left() + (values.size() == 1 ? area.width() / 2.0 :
                        i * area.width() / double(values.size() - 1));
            double y = area.bottom() - qBound(0.0, values[i], 100.0) * area.height() / 100.0;
            p.setBrush(color);
            p.setPen(Qt::NoPen);
            p.drawEllipse(QPointF(x, y), 3.5, 3.5);
        }
    };

    drawSeries(accuracyValues, QColor("#a78bfa"));
    drawSeries(gameSenseValues, QColor("#22d3ee"));

    p.setPen(QColor("#a78bfa"));
    p.drawText(left, height() - 8, "● Accuracy");
    p.setPen(QColor("#22d3ee"));
    p.drawText(left + 115, height() - 8, "● Game sense");
}
