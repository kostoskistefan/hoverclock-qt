#include "hover_clock.h"
#include "ui_hover_clock.h"

HoverClock::HoverClock(QWidget *parent) : QMainWindow(parent), ui(new Ui::HoverClock)
{
    ui->setupUi(this);
    initializeSettings();

    setStyleSheet("background:transparent;");

    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint |
                   Qt::X11BypassWindowManagerHint);

    startTimer(1000);

    resizeWindow();

    updateClockPosition();
    createSystemTray();
}

HoverClock::~HoverClock()
{
    delete ui;
}

void HoverClock::resizeWindow()
{
    int timeWidth = getTextWidth(QTime::currentTime().toString(settings["timeFormat"].toString()),
                                 settings["timeFont"].value<QFont>());

    int dateWidth = getTextWidth(QDate::currentDate().toString(settings["dateFormat"].toString()),
                                 settings["dateFont"].value<QFont>());

    resize(qMax(timeWidth, dateWidth) + PAINT_OFFSET * 2,
           settings["timeFont"].value<QFont>().pointSize() +
           settings["dateFont"].value<QFont>().pointSize() + PAINT_OFFSET * 3);
}

void HoverClock::initializeSettings()
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

void HoverClock::updateClockPosition()
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

void HoverClock::createSystemTray()
{
    QAction *exitAction = new QAction("Quit", this);
    connect(exitAction, &QAction::triggered, this, &QCoreApplication::quit);

    QAction *hideAction = new QAction("Toggle visibility", this);
    connect(hideAction, &QAction::triggered, this, &HoverClock::toggleVisibility);

    QAction *optionsAction = new QAction("Options", this);
    connect(optionsAction, &QAction::triggered, this, &HoverClock::showOptions);

    QMenu *trayMenu = new QMenu(this);
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

void HoverClock::showOptions()
{
    SettingsDialog *settingsDialog = new SettingsDialog(nullptr, &settings);
    settingsDialog->setModal(true);

    connect(settingsDialog, &SettingsDialog::updateClock, this, &HoverClock::updateClockPosition);

    settingsDialog->exec();
}

void HoverClock::toggleVisibility()
{
    setVisible(!isVisible());
}

void HoverClock::timerEvent(QTimerEvent * event)
{
    Q_UNUSED(event);
    repaint();
}

void HoverClock::paintEvent(QPaintEvent * event)
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

    QPainterPath path;

    if (settings["showTime"].toInt() == 2)
    {
        int timeStringWidth = getTextWidth(time, settings["timeFont"].value<QFont>());

        path.addText((rect().width() - timeStringWidth) / 2,
                     settings["timeFont"].value<QFont>().pointSize() + PAINT_OFFSET,
                     settings["timeFont"].value<QFont>(),
                     time);
    }

    if (settings["showDate"].toInt() == 2)
    {
        int dateStringWidth = getTextWidth(date, settings["dateFont"].value<QFont>());

        path.addText((rect().width() - dateStringWidth) / 2,
                     settings["timeFont"].value<QFont>().pointSize() +
                     settings["dateFont"].value<QFont>().pointSize() + PAINT_OFFSET * 2,
                     settings["dateFont"].value<QFont>(),
                     date);
    }

    painter.fillPath(path, QBrush(QColor(settings["fillColor"].value<QColor>())));
    painter.strokePath(path, QPen(QColor(settings["strokeColor"].value<QColor>()),
                                  settings["strokeThickness"].toFloat() + 0.0001));
}

float HoverClock::getTextWidth(QString text, QFont font)
{
    QFontMetrics metrics(font);
    return metrics.horizontalAdvance(text);
}
