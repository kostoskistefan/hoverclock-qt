#include "hoverclock.h"
#include "ui_hoverclock.h"

Hoverclock::Hoverclock(QWidget *parent) : QMainWindow(parent), ui(new Ui::Hoverclock)
{
    ui->setupUi(this);

    setWindowIcon(QPixmap(":/resources/icons/hoverclock.svg"));

    initializeSettings();

    applicationBlacklist = new QStringList();

    if(settings["enableBlacklist"].toInt() == Qt::CheckState::Checked)
    {
        focusEvent = new FocusEvent();
        qApp->installNativeEventFilter(focusEvent);

        connect(focusEvent, &FocusEvent::windowFocusChanged, this, &Hoverclock::checkBlacklistApplication);
    }

    if(settings["enableCalendar"].toInt() == Qt::CheckState::Checked)
        calendarDialog = new QCalendarWidget();

    QGuiApplication *application = qobject_cast<QGuiApplication *>(qApp);

    connect(application, &QGuiApplication::screenAdded, this, &Hoverclock::setSelectedScreen);
    connect(application, &QGuiApplication::screenRemoved, this, &Hoverclock::setSelectedScreen);

    setSelectedScreen();

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

void Hoverclock::setSelectedScreen()
{
    if(selectedScreen != nullptr)
        disconnect(selectedScreen, &QScreen::geometryChanged, this, &Hoverclock::updateClock);

    int screenCount = QGuiApplication::screens().count();
    int selectedScreenIndex = settings["selectedScreen"].toInt();

    if(selectedScreenIndex < screenCount)
    {
        QList<QScreen *> availableScreens = QGuiApplication::screens();
        selectedScreen = availableScreens[selectedScreenIndex];
    }

    else selectedScreen = QGuiApplication::primaryScreen();

    connect(selectedScreen, &QScreen::geometryChanged, this, &Hoverclock::updateClock);

    updateClock();
}

void Hoverclock::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(!calendarDialog->isVisible())
        {
            calendarDialog->show();
            calendarDialog->setGeometry(
                        QStyle::alignedRect(
                            Qt::LeftToRight,
                            Qt::AlignCenter,
                            calendarDialog->size(),
                            selectedScreen->availableGeometry()));
        }

        else calendarDialog->close();
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
        if(clockIsVisible)
        {
            bool applicationInBlacklist = false;

            for (QString& application : (*applicationBlacklist))
            {
                if(windowName.contains(application, Qt::CaseInsensitive))
                {
                    applicationInBlacklist = true;
                    break;
                }
            }

            setVisible(!applicationInBlacklist);
        }
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
    configuration.setValue("selectedScreen", configuration.value("selectedScreen", 0).toInt());
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

    int screenWidth = selectedScreen->geometry().width() + selectedScreen->geometry().x();
    int screenHeight = selectedScreen->geometry().height() + selectedScreen->geometry().y();

    QPoint *position = new QPoint(settings["horizontalPadding"].toInt(), settings["verticalPadding"].toInt());

    switch (settings["position"].toInt())
    {
        case ClockPosition::TOP_LEFT:
            position->setX(selectedScreen->geometry().x() + settings["horizontalPadding"].toInt());
            break;
        case ClockPosition::TOP_RIGHT:
            position->setX(screenWidth - width() - settings["horizontalPadding"].toInt());
            break;
        case ClockPosition::BOTTOM_LEFT:
            position->setX(selectedScreen->geometry().x() + settings["horizontalPadding"].toInt());
            position->setY(screenHeight - height() - settings["verticalPadding"].toInt());
            break;
        case ClockPosition::BOTTOM_RIGHT:
            position->setX(screenWidth - width() - settings["horizontalPadding"].toInt());
            position->setY(screenHeight - height() - settings["verticalPadding"].toInt());
            break;
    }

    move(*position);

    repaint();
}

void Hoverclock::createSystemTray()
{
    if (QSystemTrayIcon::isSystemTrayAvailable())
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
        
        tray->setIcon(QPixmap(":/resources/icons/hoverclock-tray.svg"));
        tray->show();
        
        connect(tray, &QSystemTrayIcon::activated, this, &Hoverclock::toggleVisibility);
    }
}

void Hoverclock::showOptions()
{
    if (settingsDialog == nullptr || !settingsDialog->isVisible())
    {
        settingsDialog = new SettingsDialog(nullptr, &settings);

        connect(settingsDialog, &SettingsDialog::updateClock, this, &Hoverclock::updateClock);
        connect(settingsDialog, &SettingsDialog::updateSelectedScreen, this, &Hoverclock::setSelectedScreen);
        connect(settingsDialog, &SettingsDialog::updateBlacklist, this, &Hoverclock::updateBlacklist);

        settingsDialog->exec();
    }

    else settingsDialog->showNormal();
}

void Hoverclock::updateTrayIcon()
{
    QPixmap iconPixmap(":/resources/icons/hoverclock-tray.svg");

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
    clockIsVisible = isVisible();
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


        dateVerticalAlignment += settings["timeFont"].value<QFont>().pointSize() + PAINT_OFFSET * 1.5;
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
