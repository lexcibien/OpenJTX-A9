#pragma once

#include <QComboBox>

class PortComboBox : public QComboBox
{
    Q_OBJECT

public:
    using QComboBox::QComboBox;

    void showPopup() override
    {
        emit popupOpened();
        QComboBox::showPopup();
    }

signals:
    void popupOpened();
};
