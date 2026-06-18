#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
} // namespace Ui

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    enum class GoodThings: std::uint8_t { Angels, DepecheMode, KDABVideos };

private:
    void doGoodThings(GoodThings which) const;

    Ui::MainWindow *ui;
};
