#include "mainWindow.h"
#include <QSettings>

int main(int argc, char *argv[])
{
    const QApplication app(argc, argv);

    QApplication::setApplicationName("OpenJTX_A9");
    QApplication::setOrganizationName("lexcibien");
    QApplication::setOrganizationDomain("lexcibien.me");
#ifdef Q_OS_MACOS
// To fix and prioritize .icns for macOS
#else
    QApplication::setWindowIcon(QIcon(":/icons/OpenJTXA9_ai.png"));
#endif

    QSettings::setDefaultFormat(QSettings::IniFormat);
    const QSettings settings;

    MainWindow window;

    window.show();

    const QFont sysFont;
    QApplication::setFont(sysFont);

    const int retCode = QApplication::exec();

    return retCode;
}
