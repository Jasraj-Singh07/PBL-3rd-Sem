#ifndef PERFORMANCECHART_H
#define PERFORMANCECHART_H

#include <QWidget>
#include <QVector>

class PerformanceChart : public QWidget
{
    Q_OBJECT
public:
    explicit PerformanceChart(QWidget *parent = nullptr);
    void setValues(const QVector<double> &accuracy,
                   const QVector<double> &gameSense);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<double> accuracyValues;
    QVector<double> gameSenseValues;
};

#endif
