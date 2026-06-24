#pragma once

#include "types.h"
#include "ui_MainWindow.h"
#include <QElapsedTimer>
#include <QLineSeries>
#include <QValueAxis>

constexpr float MAX_A9_READ_VOLTAGE = 20.0;
constexpr float MAX_A9_READ_CURRENT = 2.5;

class CurveHistoryTab
{
private:
    static constexpr float RANGE_TIME = 10.0;
    static constexpr double MS_TO_SECONDS = 1000.0;
    QElapsedTimer graphTimer;

    QChart *chartCurve = new QChart;
    QLineSeries *voltageSeries = new QLineSeries;
    QLineSeries *currentSeries = new QLineSeries;

    QValueAxis *axisTime = new QValueAxis;
    QValueAxis *axisVoltage = new QValueAxis;
    QValueAxis *axisCurrent = new QValueAxis;

    CurveHistoryPage curveHistoryPage;

    static void setAxisRange(const QLineSeries *series, QValueAxis *axis)
    {
        if (QList<QPointF> points = series->points(); !points.isEmpty()) {
            auto maxPoint = std::ranges::max_element(points, [](const QPointF &point1, const QPointF &point2) { return point1.y() < point2.y(); });
            qreal maxRealValue = maxPoint->y();
            axis->setRange(0, static_cast<int>(maxRealValue + 1));
        }
    }

public:
    CurveHistoryTab() { graphTimer.start(); }

    void setState(const CurveHistoryPage &value) { curveHistoryPage = value; }

    void configureGraph(Ui::MainWindow *uic) const
    {
        uic->voltageGaugeWidget->setRange(0.0, MAX_A9_READ_VOLTAGE);
        uic->currentGaugeWidget->setRange(0.0, MAX_A9_READ_CURRENT);

        voltageSeries->setName("Tensão");
        currentSeries->setName("Corrente");

        voltageSeries->setColor(Qt::green);
        currentSeries->setColor(Qt::red);

        chartCurve->addSeries(voltageSeries);
        chartCurve->addSeries(currentSeries);

        axisTime->setTitleText("Tempo (s)");
        axisVoltage->setTitleText("Tensão (V)");
        axisCurrent->setTitleText("Corrente");

        axisTime->setRange(0, RANGE_TIME);
        axisVoltage->setRange(0, MAX_A9_READ_VOLTAGE);
        axisCurrent->setRange(0, MAX_A9_READ_CURRENT);

        chartCurve->addAxis(axisTime, Qt::AlignBottom);
        chartCurve->addAxis(axisVoltage, Qt::AlignLeft);
        chartCurve->addAxis(axisCurrent, Qt::AlignRight);

        voltageSeries->attachAxis(axisTime);
        voltageSeries->attachAxis(axisVoltage);

        currentSeries->attachAxis(axisTime);
        currentSeries->attachAxis(axisCurrent);

        uic->curveHistoryGraph->setChart(chartCurve);
    }

    void update(Ui::MainWindow *uic)
    {
        double time = static_cast<double>(graphTimer.elapsed()) / MS_TO_SECONDS;

        uic->voltageGaugeWidget->setValue(curveHistoryPage.voltage.toDouble());
        uic->currentGaugeWidget->setValue(curveHistoryPage.current.toDouble());

        voltageSeries->append(time, curveHistoryPage.voltage.toDouble());
        currentSeries->append(time, curveHistoryPage.current.toDouble());

        setAxisRange(currentSeries, axisCurrent);
        setAxisRange(voltageSeries, axisVoltage);
        axisTime->setRange(std::max(0.0, time - RANGE_TIME), time);

        while (!voltageSeries->points().isEmpty() && voltageSeries->points().front().x() < time - RANGE_TIME) {
            voltageSeries->removePoints(0, 1);
        }

        while (!currentSeries->points().isEmpty() && currentSeries->points().front().x() < time - RANGE_TIME) {
            currentSeries->removePoints(0, 1);
        }
    }
};
