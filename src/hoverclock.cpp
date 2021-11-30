#include "hoverclock.h"
#include "ui_hoverclock.h"

Hoverclock::Hoverclock(QWidget *parent) : QMainWindow(parent), ui(new Ui::Hoverclock)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/resources/icons/hoverclock.png"));

    initializeSettings();

    applicationBlacklist = new QStringList();

    if(settings["enableBlacklist"].toInt() == Qt::CheckState::Checked)
    {
        focusEvent = new FocusEvent();
        qApp->installNativeEventFilter(focusEvent);

        connect(focusEvent, SIGNAL(windowFocusChanged(QString)), this, SLOT(checkBlacklistApplication(QString)));
    }

    if(settings["enableCalendar"].toInt() == Qt::CheckState::Checked)
        calendarDialog = new QCalendarWidget();

    makeWindowTransparent();

    updateBlacklist();
    updateClock();
    createSystemTray();

    startTimer(1000);
}

Hoverclock::~Hoverclock()
{
    delete ui;
}

void Hoverclock::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        calendarDialog->show();
        calendarDialog->setGeometry(
                    QStyle::alignedRect(
                        Qt::LeftToRight,
                        Qt::AlignCenter,
                        calendarDialog->size(),
                        QGuiApplication::primaryScreen()->availableGeometry()));
    }
}

void Hoverclock::makeWindowTransparent()
{
    setStyleSheet("background:transparent;");

    if(settings["enableCalendar"].toInt() == Qt::CheckState::Unchecked)
        setAttribute(Qt::WA_TransparentForMouseEvents);

    setAttribute(Qt::WA_TranslucentBackground, true);

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint |
                   Qt::X11BypassWindowManagerHint);
}

void Hoverclock::checkBlacklistApplication(QString windowName)
{
    if(settings["enableBlacklist"].toInt() == Qt::CheckState::Checked)
    {
        bool applicationInBlacklist = false;

        for (QString& application : (*applicationBlacklist))
            if(windowName.contains(application, Qt::CaseInsensitive))
                applicationInBlacklist = true;

        setVisible(!applicationInBlacklist);
    }
}

void Hoverclock::resizeWindow()
{
    int timeWidth = getTextWidth(QTime::currentTime().toString(settings["timeFormat"].toString()),
                                 settings["timeFont"].value<QFont>());

    int dateWidth = getTextWidth(QDate::currentDate().toString(settings["dateFormat"].toString()),
                                 settings["dateFont"].value<QFont>());

    int windowWidth = 0;
    int windowHeight = 0;

    if (settings["showTime"].toInt() == Qt::CheckState::Checked)
    {
        windowWidth += timeWidth + PAINT_OFFSET * 2;
        windowHeight += settings["timeFont"].value<QFont>().pointSize() + PAINT_OFFSET * 2;
    }

    if (settings["showDate"].toInt() == Qt::CheckState::Checked)
    {
        if(windowWidth == 0 || dateWidth > timeWidth)
            windowWidth = dateWidth + PAINT_OFFSET * 2;

        windowHeight += settings["dateFont"].value<QFont>().pointSize() + PAINT_OFFSET * 2;
    }

    resize(windowWidth, windowHeight);
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
    configuration.setValue("timeZone", configuration.value("timeZone", QTimeZone::systemTimeZoneId()).toByteArray());
    configuration.setValue("fillColor", configuration.value("fillColor", QColor("white")).value<QColor>());
    configuration.setValue("timeFormat", configuration.value("timeFormat", "hh:mm").toString());
    configuration.setValue("dateFormat", configuration.value("dateFormat", "dd MMM yyyy").toString());
    configuration.setValue("strokeColor", configuration.value("strokeColor", QColor("darkGray")).value<QColor>());
    configuration.setValue("enableCalendar", configuration.value("enableCalendar", Qt::CheckState::Unchecked).toInt());
    configuration.setValue("strokeThickness", configuration.value("strokeThickness", 1.5).toFloat());
    configuration.setValue("verticalPadding", configuration.value("verticalPadding", 50).toInt());
    configuration.setValue("enableBlacklist", configuration.value("enableBlacklist", Qt::CheckState::Unchecked).toInt());
    configuration.setValue("horizontalPadding", configuration.value("horizontalPadding", 50).toInt());
    configuration.setValue("applicationBlacklist", configuration.value("applicationBlacklist", "").toString());

    QStringList keys = configuration.allKeys();

    for (QString& key : keys)
        settings[key] = configuration.value(key);
}

void Hoverclock::updateBlacklist()
{
    applicationBlacklist->clear();

    QString applications = settings["applicationBlacklist"].toString();

    if(!applications.isEmpty())
    {
        QStringList applicationList = applications.split(",");

        for (QString& application: applicationList)
            applicationBlacklist->append(application.trimmed());
    }
}

void Hoverclock::updateClock()
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

    trayMenu->addAction(hideAction);
    trayMenu->addAction(optionsAction);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);

    tray = new QSystemTrayIcon(this);
    tray->setContextMenu(trayMenu);

    QIcon icon(":/resources/icons/hoverclock.png");
    icon.setIsMask(true);

    tray->setIcon(icon);
    tray->show();

    connect(tray, &QSystemTrayIcon::activated, this, &Hoverclock::toggleVisibility);
}

void Hoverclock::showOptions()
{
    if (settingsDialog == nullptr || !settingsDialog->isVisible())
    {
        settingsDialog = new SettingsDialog(nullptr, &settings);

        connect(settingsDialog, &SettingsDialog::updateClock, this, &Hoverclock::updateClock);
        connect(settingsDialog, &SettingsDialog::updateBlacklist, this, &Hoverclock::updateBlacklist);

        settingsDialog->exec();
    }

    else settingsDialog->showNormal();
}

void Hoverclock::updateTrayIcon()
{
    QPixmap iconPixmap(":/resources/icons/hoverclock.png");

    if(!isVisible())
    {
        QPixmap disabledIconPixmap(iconPixmap.size());
        disabledIconPixmap.fill(Qt::transparent);

        QPainter painter(&disabledIconPixmap);

        painter.setOpacity(0.4);
        painter.drawPixmap(0, 0, iconPixmap);

        painter.end();

        tray->setIcon(disabledIconPixmap);
    }

    else tray->setIcon(iconPixmap);
}

void Hoverclock::toggleVisibility()
{
    setVisible(!isVisible());
    updateTrayIcon();
}

void Hoverclock::timerEvent(QTimerEvent * event)
{
    Q_UNUSED(event);
    repaint();
}

void Hoverclock::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    QDateTime currentDateTime = QDateTime::currentDateTime();

    QTimeZone timeZone(settings["timeZone"].toByteArray());

    QDateTime dateTimeInTimeZone = currentDateTime.toTimeZone(timeZone);

    QString time = dateTimeInTimeZone.time().toString(settings["timeFormat"].toString()); // QTime::currentTime().toString(settings["timeFormat"].toString());
    QString date = dateTimeInTimeZone.date().toString(settings["dateFormat"].toString()); //QDate::currentDate().toString(settings["dateFormat"].toString());

    QPainter painter(this);
    painter.setOpacity(settings["opacity"].toFloat());
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform |
                           QPainter::HighQualityAntialiasing);

    QPainterPath canvas;

    int dateVerticalAlignment = settings["dateFont"].value<QFont>().pointSize() + PAINT_OFFSET;

    if (settings["showTime"].toInt() == Qt::CheckState::Checked)
    {
        canvas.addText((rect().width() - getTextWidth(time, settings["timeFont"].value<QFont>())) / 2,
                     settings["timeFont"].value<QFont>().pointSize() + PAINT_OFFSET,
                     settings["timeFont"].value<QFont>(),
                     time);


        dateVerticalAlignment += settings["timeFont"].value<QFont>().pointSize() + PAINT_OFFSET * 2;
    }

    if (settings["showDate"].toInt() == Qt::CheckState::Checked)
    {
        canvas.addText((rect().width() - getTextWidth(date, settings["dateFont"].value<QFont>())) / 2,
                     dateVerticalAlignment,
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
