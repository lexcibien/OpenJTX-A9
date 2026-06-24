#pragma once

#include "types.h"
#include "ui_MainWindow.h"
#include <QElapsedTimer>
#include <QLineSeries>
#include <QValueAxis>

constexpr float RANGE_TIME_VOLTAGE_GEAR = 20.0;

class VoltageTab
{
    private:
    static constexpr double MS_TO_SECONDS = 1000.0;
    QElapsedTimer graphTimer;

    QChart *chartVoltageGearPage = new QChart;
    QLineSeries *voltageGearSeries = new QLineSeries;

    QValueAxis *axisXVoltageGearTime = new QValueAxis;
    QValueAxis *axisYVoltageGear = new QValueAxis;

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
        voltageGearSeries->setName("Tensão");

        voltageGearSeries->setColor(Qt::green);

        chartVoltageGearPage->addSeries(voltageGearSeries);

        axisXVoltageGearTime->setTitleText("Tempo (s)");
        axisYVoltageGear->setTitleText("Tensão (V)");

        axisXVoltageGearTime->setRange(0, RANGE_TIME_VOLTAGE_GEAR);
        axisYVoltageGear->setRange(0, 1.0);

        chartVoltageGearPage->addAxis(axisXVoltageGearTime, Qt::AlignBottom);
        chartVoltageGearPage->addAxis(axisYVoltageGear, Qt::AlignLeft);

        voltageGearSeries->attachAxis(axisXVoltageGearTime);
        voltageGearSeries->attachAxis(axisYVoltageGear);

        uic->voltageGearGraph->setChart(chartVoltageGearPage);
    }

    void update(Ui::MainWindow *uic)
    {
        double time = static_cast<double>(graphTimer.elapsed()) / MS_TO_SECONDS;

        uic->voltageGearDisplay->setText(voltagePage.voltage);

        voltageGearSeries->append(time, voltagePage.voltage.toDouble());

        axisXVoltageGearTime->setRange(std::max(0.0, time - RANGE_TIME_VOLTAGE_GEAR), time);

        // set the y range based on max value
        setAxisRange(voltageGearSeries, axisYVoltageGear);

        while (!voltageGearSeries->points().isEmpty() && voltageGearSeries->points().front().x() < time - RANGE_TIME_VOLTAGE_GEAR) {
            voltageGearSeries->removePoints(0, 1);
        }
    }
};
