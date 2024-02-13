#include "hoverclock-qt.h"
#include "settings_dialog.h"

#include <QApplication>
#include <run_guard.h>
#include <QFile>
#include <QDebug>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char *argv[])
{
    RunGuard guard("hoverclock-qt");

    if (!guard.tryToRun())
    {
        qCritical() << "An instance is already running.";
        return 2;
    }

    QApplication application(argc, argv);

    application.setWindowIcon(QIcon(":/resources/icons/hoverclock-qt.svg"));

    application.setApplicationDisplayName("Hoverclock QT");
    application.setOrganizationDomain("https://github.com/kostoskistefan/hoverclock-qt");
    application.setOrganizationName("hoverclock-qt");
    application.setApplicationName("hoverclock-qt");
    application.setApplicationVersion(APP_VERSION);

    QFile themeFile(":/resources/styles/dark-theme.qss");
    themeFile.open(QFile::ReadOnly);
    QString theme = QLatin1String(themeFile.readAll());
    application.setStyleSheet(theme);

    QCommandLineParser parser;
    QCommandLineOption settings(QStringList() << "c" << "configure", "Launches Hoverclock's configuration window.");

    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(settings);

    parser.process(application);

    Hoverclock hoverclock;

    if (parser.isSet("configure"))
        hoverclock.showOptions();

    else hoverclock.show();

    return application.exec();
}
