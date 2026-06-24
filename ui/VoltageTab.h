#pragma once

#include "types.h"
#include "ui_MainWindow.h"
#include <QElapsedTimer>
#include <QLineSeries>
#include <QValueAxis>

class VoltageTab
{
private:
    static constexpr float RANGE_TIME = 20.0;
    static constexpr double MS_TO_SECONDS = 1000.0;
    QElapsedTimer graphTimer;

    QChart *chartVoltage = new QChart;
    QLineSeries *voltageSeries = new QLineSeries;

    QValueAxis *axisTime = new QValueAxis;
    QValueAxis *axisVoltage = new QValueAxis;

    VoltagePage voltagePage;

    static void setAxisRange(const QLineSeries *series, QValueAxis *axis)
    {
        if (QList<QPointF> points = series->points(); !points.isEmpty()) {
            auto maxPoint = std::ranges::max_element(points, [](const QPointF &point1, const QPointF &point2) { return point1.y() < point2.y(); });
            qreal maxRealValue = maxPoint->y();
            axis->setRange(0, static_cast<int>(maxRealValue + 1));
        }
    }

public:
    VoltageTab() { graphTimer.start(); }

    void setState(const VoltagePage &value) { voltagePage = value; }

    void configureGraph(Ui::MainWindow *uic) const
    {
        voltageSeries->setName("Tensão");

        voltageSeries->setColor(Qt::green);

        chartVoltage->addSeries(voltageSeries);

        axisTime->setTitleText("Tempo (s)");
        axisVoltage->setTitleText("Tensão (V)");

        axisTime->setRange(0, RANGE_TIME);
        axisVoltage->setRange(0, 1.0);

        chartVoltage->addAxis(axisTime, Qt::AlignBottom);
        chartVoltage->addAxis(axisVoltage, Qt::AlignLeft);

        voltageSeries->attachAxis(axisTime);
        voltageSeries->attachAxis(axisVoltage);

        uic->voltageGearGraph->setChart(chartVoltage);
    }

    void update(Ui::MainWindow *uic)
    {
        double time = static_cast<double>(graphTimer.elapsed()) / MS_TO_SECONDS;

        uic->voltageGearDisplay->setText(voltagePage.voltage);

        voltageSeries->append(time, voltagePage.voltage.toDouble());

        axisTime->setRange(std::max(0.0, time - RANGE_TIME), time);

        // set the y range based on max value
        setAxisRange(voltageSeries, axisVoltage);

        while (!voltageSeries->points().isEmpty() && voltageSeries->points().front().x() < time - RANGE_TIME) {
            voltageSeries->removePoints(0, 1);
        }
    }
};
