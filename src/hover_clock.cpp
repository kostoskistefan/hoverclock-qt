#include "hover_clock.h"
#include "ui_hover_clock.h"

Hoverclock::Hoverclock(QWidget *parent) : QMainWindow(parent), ui(new Ui::Hoverclock)
{
    ui->setupUi(this);

    applicationBlacklist = new QStringList();

    initializeSettings();

    makeWindowTransparent();

    resizeWindow();

    updateClockPosition();
    createSystemTray();

    startTimer(1000);

    process = new QProcess(this);

    connect(process, &QProcess::readyReadStandardOutput, this, [=](){
        checkBlacklistApplication(getFocusedApplicationName());
    });

    process->start("/usr/bin/xprop", QStringList() << "-spy" << "-root" << "_NET_ACTIVE_WINDOW");
}

Hoverclock::~Hoverclock()
{
    delete ui;
}

void Hoverclock::makeWindowTransparent()
{
    setStyleSheet("background:transparent;");

    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint |
                   Qt::X11BypassWindowManagerHint);
}

QString Hoverclock::getFocusedApplicationName()
{
    Display *display;
    Window focus;
    int revert;

    display = XOpenDisplay(NULL);
    XGetInputFocus(display, &focus, &revert);

    XClassHint classHint;
    XGetClassHint(display, focus, &classHint);

    QString windowName = reinterpret_cast<char *>(classHint.res_name);

    return windowName;
}

void Hoverclock::checkBlacklistApplication(QString windowName)
{
    bool applicationInBlacklist = false;

    for (QString& application : (*applicationBlacklist))
        if(windowName.contains(application, Qt::CaseInsensitive))
            applicationInBlacklist = true;

    setVisible(!applicationInBlacklist);
}

void Hoverclock::resizeWindow()
{
    int timeWidth = getTextWidth(QTime::currentTime().toString(settings["timeFormat"].toString()),
                                 settings["timeFont"].value<QFont>());

    int dateWidth = getTextWidth(QDate::currentDate().toString(settings["dateFormat"].toString()),
                                 settings["dateFont"].value<QFont>());

    resize(qMax(timeWidth, dateWidth) + PAINT_OFFSET * 2,
           settings["timeFont"].value<QFont>().pointSize() +
           settings["dateFont"].value<QFont>().pointSize() + PAINT_OFFSET * 3);
}

void Hoverclock::initializeSettings()
{
    QSettings configuration;

    configuration.setValue("opacity", configuration.value("opacity", 0.5).toFloat());
    configuration.setValue("showTime", configuration.value("showTime", 2).toInt());
    configuration.setValue("showDate", configuration.value("showDate", 2).toInt());
    configuration.setValue("timeFont", configuration.value("timeFont", QFont("Montserrat Medium", 12)).value<QFont>());
    configuration.setValue("dateFont", configuration.value("dateFont", QFont("Montserrat Medium", 7)).value<QFont>());
    configuration.setValue("position", configuration.value("position", ClockPosition::BOTTOM_RIGHT).toInt());
    configuration.setValue("fillColor", configuration.value("fillColor", QColor("white")).value<QColor>());
    configuration.setValue("timeFormat", configuration.value("timeFormat", "hh:mm").toString());
    configuration.setValue("dateFormat", configuration.value("dateFormat", "dd.MM.yyyy").toString());
    configuration.setValue("strokeColor", configuration.value("strokeColor", QColor("darkGray")).value<QColor>());
    configuration.setValue("strokeThickness", configuration.value("strokeThickness", 1.5).toFloat());
    configuration.setValue("verticalPadding", configuration.value("verticalPadding", 50).toInt());
    configuration.setValue("horizontalPadding", configuration.value("horizontalPadding", 50).toInt());

    QStringList keys = configuration.allKeys();

    for (QString& key : keys)
        settings[key] = configuration.value(key);
}

void Hoverclock::updateClockPosition()
{
    resizeWindow();

    int screenWidth = QGuiApplication::primaryScreen()->geometry().width();
    int screenHeight = QGuiApplication::primaryScreen()->geometry().height();

    switch (settings["position"].toInt())
    {
        case ClockPosition::TOP_LEFT:
            move(settings["horizontalPadding"].toInt(),
                 settings["verticalPadding"].toInt());
            break;
        case ClockPosition::TOP_RIGHT:
            move(screenWidth - width() - settings["horizontalPadding"].toInt(),
                 settings["verticalPadding"].toInt());
            break;
        case ClockPosition::BOTTOM_LEFT:
            move(settings["horizontalPadding"].toInt(),
                 screenHeight - height() - settings["verticalPadding"].toInt());
            break;
        case ClockPosition::BOTTOM_RIGHT:
            move(screenWidth - width() - settings["horizontalPadding"].toInt(),
                 screenHeight - height() - settings["verticalPadding"].toInt());
            break;
    }

    repaint();
}

void Hoverclock::createSystemTray()
{
    QAction *exitAction = new QAction("Quit", this);
    connect(exitAction, &QAction::triggered, this, &QCoreApplication::quit);

    QAction *hideAction = new QAction("Toggle visibility", this);
    connect(hideAction, &QAction::triggered, this, &Hoverclock::toggleVisibility);

    QAction *optionsAction = new QAction("Options", this);
    connect(optionsAction, &QAction::triggered, this, &Hoverclock::showOptions);

    QMenu *trayMenu = new QMenu(this);

//    addActionToTrayMenu(trayMenu, "Toggle visibility", QFunctionPointer(&Hoverclock::toggleVisibility));
//    addActionToTrayMenu(trayMenu, "Options", &Hoverclock::showOptions);
//    addActionToTrayMenu(trayMenu, "Quit", &QCoreApplication::quit);

    trayMenu->addAction(hideAction);
    trayMenu->addAction(optionsAction);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);

    QSystemTrayIcon *tray = new QSystemTrayIcon(this);
    tray->setContextMenu(trayMenu);

    QPixmap iconPixmap(":/icons/resources/icon.svg");
    QIcon icon(iconPixmap);
    icon.setIsMask(true);

    tray->setIcon(icon);
    tray->show();

    connect(tray, &QSystemTrayIcon::activated, this, [=]() {
        setVisible(!isVisible());
    });
}

void Hoverclock::showOptions()
{
    SettingsDialog *settingsDialog = new SettingsDialog(nullptr, &settings);
    settingsDialog->setModal(true);

    connect(settingsDialog, &SettingsDialog::updateClock, this, &Hoverclock::updateClockPosition);

    settingsDialog->exec();
}

void Hoverclock::toggleVisibility()
{
    setVisible(!isVisible());
}

void Hoverclock::timerEvent(QTimerEvent * event)
{
    Q_UNUSED(event);
    repaint();
}

void Hoverclock::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    QString time = QTime::currentTime().toString(settings["timeFormat"].toString());
    QString date = QDate::currentDate().toString(settings["dateFormat"].toString());

    QPainter painter(this);
    painter.setOpacity(settings["opacity"].toFloat());
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform |
                           QPainter::HighQualityAntialiasing);

    QPainterPath canvas;

    if (settings["showTime"].toInt() == 2)
    {
        int textWidth = getTextWidth(time, settings["timeFont"].value<QFont>());

        canvas.addText((rect().width() - textWidth) / 2,
                     settings["timeFont"].value<QFont>().pointSize() + PAINT_OFFSET,
                     settings["timeFont"].value<QFont>(),
                     time);
    }

    if (settings["showDate"].toInt() == 2)
    {
        int textWidth = getTextWidth(date, settings["dateFont"].value<QFont>());

        canvas.addText((rect().width() - textWidth) / 2,
                     settings["timeFont"].value<QFont>().pointSize() +
                     settings["dateFont"].value<QFont>().pointSize() + PAINT_OFFSET * 2,
                     settings["dateFont"].value<QFont>(),
                     date);
    }

    painter.fillPath(canvas, QBrush(QColor(settings["fillColor"].value<QColor>())));
    painter.strokePath(canvas, QPen(QColor(settings["strokeColor"].value<QColor>()),
                                  settings["strokeThickness"].toFloat() + 0.0001));
}

float Hoverclock::getTextWidth(QString text, QFont font)
{
    QFontMetrics metrics(font);
    return metrics.horizontalAdvance(text);
}
