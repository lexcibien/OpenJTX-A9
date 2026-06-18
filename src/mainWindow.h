#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
} // namespace Ui

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;

    enum class BaudRateValues : uint { BAUD_9600 = 9600, BAUD_19200 = 19200, BAUD_38400 = 38400, BAUD_115200 = 115200, BAUD_230400 = 230400 };
    QVariant baudRate = 0;
    void createComboBaudRate();

    void connectButtons() const;
    void changeText();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void comboBaudRate(BaudRateValues which);
};
