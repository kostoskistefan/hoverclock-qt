#ifndef HOVERCLOCK_H
#define HOVERCLOCK_H

#include <QTime>
#include <QMenu>
#include <QTimer>
#include <QScreen>
#include <QPainter>
#include <QPalette>
#include <QProcess>
#include <QSettings>
#include <x11event.h>
#include <QMainWindow>
#include <QPainterPath>
#include <QSystemTrayIcon>
#include <clock_position.h>
#include <settings_dialog.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Hoverclock; }
QT_END_NAMESPACE

#define PAINT_OFFSET 3

class Hoverclock : public QMainWindow
{
    Q_OBJECT

    public:
        Hoverclock(QWidget *parent = nullptr);
        ~Hoverclock();

    protected:
        void timerEvent(QTimerEvent *event);
        void paintEvent(QPaintEvent *event);

    private:
        Ui::Hoverclock *ui;

        X11Event *x11Event;
        QProcess *windowIdentifier;
        QString focusedApplicationTitle;
        QStringList *applicationBlacklist;
        QHash<QString, QVariant> settings;
        SettingsDialog *settingsDialog = nullptr;

        void updateClock();
        void showOptions();
        void resizeWindow();
        void createSystemTray();
        void initializeSettings();
        void makeWindowTransparent();
        float getTextWidth(QString text, QFont font);


    private slots:
        void toggleVisibility();
        void checkBlacklistApplication(QString windowName);
};

#endif // HOVERCLOCK_H
