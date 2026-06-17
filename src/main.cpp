#include "mainWindow.h"
#include <QApplication>
#include <QFileOpenEvent>
#include <QMainWindow>
#include <QSettings>
#include <QString>

// NOLINTNEXTLINE (modernize-use-trailing-return-type)
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("OpenJTX_A9");
    QApplication::setOrganizationName("lexcibien");
    QApplication::setOrganizationDomain("lexcibien.me");

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    MainWindow window;

    window.show();

    int fontSize = settings.value("Main/FontSize", 9).toInt();
    QFont sysFont;
    sysFont.setPointSize(fontSize);
    QApplication::setFont(sysFont);

    int retCode = QApplication::exec();

    return retCode;
}
