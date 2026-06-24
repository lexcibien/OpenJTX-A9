/*
 *   Copyright 2008-2012 Meinert Jordan <meinert@gmx.at>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

// The link for this library can be found at https://www.gnome-look.org/p/1132178

#ifndef QSCALE_H
#define QSCALE_H

#include <QWidget>

class QScale : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue)
    Q_PROPERTY(double min READ minimum WRITE setMinimum)
    Q_PROPERTY(double max READ maximum WRITE setMaximum)
    Q_PROPERTY(Qt::Orientations orientation READ orientations WRITE setOrientations)
    Q_PROPERTY(bool scaleVisible READ isScaleVisible WRITE setScaleVisible)
    Q_PROPERTY(bool labelVisible READ isLabelsVisible WRITE setLabelsVisible)

public:
    explicit QScale(QWidget *parent = nullptr);
    ~QScale() override = default;

    [[nodiscard]] double minimum() const;
    [[nodiscard]] double maximum() const;

    [[nodiscard]] double value() const;

    void setLabelsVisible(bool);
    [[nodiscard]] bool isLabelsVisible() const;

    void setScaleVisible(bool);
    [[nodiscard]] bool isScaleVisible() const;

    void setBorderWidth(int);
    [[nodiscard]] int borderWidth() const;

    void setLabelsFormat(char format, int precision = -1);

    [[nodiscard]] double majorStepSize() const;
    [[nodiscard]] int minorStepCount() const;

    void setInvertedAppearance(bool invert);
    [[nodiscard]] bool invertedAppearance() const;

    [[nodiscard]] Qt::Orientations orientations() const;

public Q_SLOTS:
    void setMinimum(double);
    void setMaximum(double);
    void setRange(double min, double max);

    void setValue(double);

    void setMajorStepSize(double);
    void setMinorStepCount(int);

    void setOrientations(Qt::Orientations);

protected:
    void resizeEvent(QResizeEvent *re) override;
    void paintEvent(QPaintEvent *pe) override;

private:
    double m_minimum;
    double m_maximum;

    double m_value;

    bool m_labelsVisible;
    bool m_scaleVisible;

    int m_borderWidth;

    char m_labelsFormat;
    int m_labelsPrecision;

    double m_majorStepSize;
    int m_minorStepCount;

    bool m_invertedAppearance;

    Qt::Orientations m_orientations;

    QPainter *painter;
    QString labelSample;

    double pi;

    void updateLabelSample();
};

#endif // QSCALE_H
