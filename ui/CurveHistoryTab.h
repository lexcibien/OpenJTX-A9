#pragma once

#include "types.h"
#include "ui_MainWindow.h"
#include <QElapsedTimer>
#include <QLineSeries>
#include <QValueAxis>

constexpr float MAX_A9_READ_VOLTAGE = 20.0;
constexpr float MAX_A9_READ_CURRENT = 2.5;
constexpr float RANGE_TIME_CURVE = 10.0;

class CurveHistoryTab
{
private:
    static constexpr double MS_TO_SECONDS = 1000.0;
    QElapsedTimer graphTimer;

    QChart *chartCurvePage = new QChart;
    QLineSeries *voltageSeries = new QLineSeries;
    QLineSeries *currentSeries = new QLineSeries;

    QValueAxis *axisXCurveTime = new QValueAxis;
    QValueAxis *axisYVoltage = new QValueAxis;
    QValueAxis *axisYCurrent = new QValueAxis;

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

        chartCurvePage->addSeries(voltageSeries);
        chartCurvePage->addSeries(currentSeries);

        axisXCurveTime->setTitleText("Tempo (s)");
        axisYVoltage->setTitleText("Tensão (V)");
        axisYCurrent->setTitleText("Corrente");

        axisXCurveTime->setRange(0, RANGE_TIME_CURVE);
        axisYVoltage->setRange(0, MAX_A9_READ_VOLTAGE);
        axisYCurrent->setRange(0, MAX_A9_READ_CURRENT);

        chartCurvePage->addAxis(axisXCurveTime, Qt::AlignBottom);
        chartCurvePage->addAxis(axisYVoltage, Qt::AlignLeft);
        chartCurvePage->addAxis(axisYCurrent, Qt::AlignRight);

        voltageSeries->attachAxis(axisXCurveTime);
        voltageSeries->attachAxis(axisYVoltage);

        currentSeries->attachAxis(axisXCurveTime);
        currentSeries->attachAxis(axisYCurrent);

        uic->curveHistoryGraph->setChart(chartCurvePage);
    }

    void update(Ui::MainWindow *uic)
    {
        double time = static_cast<double>(graphTimer.elapsed()) / MS_TO_SECONDS;

        uic->voltageGaugeWidget->setValue(curveHistoryPage.voltage.toDouble());
        uic->currentGaugeWidget->setValue(curveHistoryPage.current.toDouble());

        voltageSeries->append(time, curveHistoryPage.voltage.toDouble());
        currentSeries->append(time, curveHistoryPage.current.toDouble());

        setAxisRange(currentSeries, axisYCurrent);
        setAxisRange(voltageSeries, axisYVoltage);
        axisXCurveTime->setRange(std::max(0.0, time - RANGE_TIME_CURVE), time);

        while (!voltageSeries->points().isEmpty() && voltageSeries->points().front().x() < time - RANGE_TIME_CURVE) {
            voltageSeries->removePoints(0, 1);
        }

        while (!currentSeries->points().isEmpty() && currentSeries->points().front().x() < time - RANGE_TIME_CURVE) {
            currentSeries->removePoints(0, 1);
        }
    }
};
