#ifndef HOVERCLOCK_H
#define HOVERCLOCK_H

#include <X11/Xutil.h>

#undef Bool
#undef CursorShape
#undef Expose
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef None
#undef Status
#undef Unsorted

#include <QTime>
#include <QMenu>
#include <QTimer>
#include <QScreen>
#include <QPainter>
#include <QPalette>
#include <QProcess>
#include <QSettings>
#include <QMainWindow>
#include <QPainterPath>
#include <QSystemTrayIcon>
#include <QAbstractNativeEventFilter>

#include "clock_position.h"
#include "settings_dialog.h"

#include <x11event.h>
#include <QDebug>

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
        QProcess *process;
        bool started = false;
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
        QString getFocusedApplicationName();
        float getTextWidth(QString text, QFont font);
        void checkBlacklistApplication(QString windowName);


    private slots:
        void toggleVisibility();

    public slots:
        void windowFocusChanged();
};

#endif // HOVERCLOCK_H
