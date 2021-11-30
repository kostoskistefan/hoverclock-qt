#ifndef HOVERCLOCK_H
#define HOVERCLOCK_H

#include <QTime>
#include <QMenu>
#include <QStyle>
#include <QTimer>
#include <QScreen>
#include <QPainter>
#include <QPalette>
#include <QProcess>
#include <QTimeZone>
#include <QSettings>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainterPath>
#include <focus_event.h>
#include <QCalendarWidget>
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
        void mousePressEvent(QMouseEvent *event);

    private:
        Ui::Hoverclock *ui;
        QSystemTrayIcon *tray;
        FocusEvent *focusEvent;
        QProcess *windowIdentifier;
        QString focusedApplicationTitle;
        QStringList *applicationBlacklist;
        QHash<QString, QVariant> settings;
        SettingsDialog *settingsDialog = nullptr;
        QCalendarWidget *calendarDialog = nullptr;

        void updateClock();
        void showOptions();
        void resizeWindow();
        void updateBlacklist();
        void createSystemTray();
        void updateTrayIcon();
        void initializeSettings();
        void makeWindowTransparent();
        float getTextWidth(QString text, QFont font);

    private slots:
        void toggleVisibility();
        void checkBlacklistApplication(QString windowName);
};

#endif // HOVERCLOCK_H
