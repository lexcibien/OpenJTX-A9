#pragma once

#include <QComboBox>

class PortComboBox : public QComboBox
{
    Q_OBJECT

public:
    using QComboBox::QComboBox;

signals:
    void popupOpened();

protected:
    void showPopup() override
    {
        emit popupOpened();
        QComboBox::showPopup();
    }
};
