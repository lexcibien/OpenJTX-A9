#pragma once

#include <QObject>

struct DiodePage
{
    enum Position : uint8_t {
        VOLTAGE_TEST,
        REGISTRY_1,
        REGISTRY_2,
        REGISTRY_3,
        REGISTRY_4,
        REGISTRY_5,
        REGISTRY_6,
    };
    QString pageName = "zd";
    QString voltageTest = "0.0000";
    QString registry_1 = "0.0000";
    QString registry_2 = "0.0000";
    QString registry_3 = "0.0000";
    QString registry_4 = "0.0000";
    QString registry_5 = "0.0000";
    QString registry_6 = "0.0000";
};

struct VoltagePage
{
    enum Position : uint8_t {
        VOLTAGE,
    };
    QString pageName = "dv";
    QString voltage = "0.0000";
};

struct ChargingUSBPage
{
    enum Position : uint8_t {
        CURRENT,
        VOLTAGE,
        POWER,
        MAH,
        MWH,
        HOUR,
    };
    QString pageName = "db";
    QString current = "0.0000"; // 60 Hz  0000.000000
    QString voltage = "0.0000";
    QString power = "0.0000";
    QString mAh = "0.0000"; // 1 Hz
    QString mWh = "0.0000";
    QString hour = "000:00:00"; // HHH:mm:ss
};

struct CurveHistoryPage
{
    enum Position : uint8_t {
        CURRENT,
        VOLTAGE
    };
    QString pageName = "qc";
    QString current = "0.0000";
    QString voltage = "0.0000";
};
