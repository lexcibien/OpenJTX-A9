#include "mainWindow.h"
#include <QApplication>
#include <QFileOpenEvent>
#include <QMainWindow>
#include <QSettings>
#include <QString>

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

    QFont sysFont;
    QApplication::setFont(sysFont);

    int retCode = QApplication::exec();

    return retCode;
}
