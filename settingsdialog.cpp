#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent, QSettings *settings) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->settings = settings;

    ui->saveButton->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton)));
    ui->cancelButton->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton)));

    connect(ui->saveButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &SettingsDialog::cancel);

    const QStringList keys = settings->allKeys();

    Q_FOREACH(QString key, keys) {
        settingsHash[key] = settings->value(key);
    }

    ui->positionComboBox->addItem("Top Left");
    ui->positionComboBox->addItem("Top Right");
    ui->positionComboBox->addItem("Bottom Left");
    ui->positionComboBox->addItem("Bottom Right");

//    ui->time (settings->value("timeFont").value<QFont>());
//    ui-> (settings->value("dateFont").value<QFont>());
//    ui->fillColorInput (settings->value("fillColor").value<QColor>());
//    ui-> settings->value("strokeColor").value<QColor>());

    ui->opacityDoubleSpinBox->setValue(settings->value("opacity").toFloat());
    ui->positionComboBox->setCurrentIndex(settings->value("position").toInt());
    ui->timeFormatLineEdit->setText(settings->value("timeFormat").toString());
    ui->dateFormatLineEdit->setText(settings->value("dateFormat").toString());
    ui->strokeThicknessSpinBox->setValue(settings->value("strokeThickness").toFloat());
    ui->verticalPaddingSpinBox->setValue(settings->value("verticalPadding").toInt());
    ui->horizontalPaddingSpinBox->setValue(settings->value("horizontalPadding").toInt());

    connect(ui->opacityDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](){
        settings->setValue("opacity", ui->opacityDoubleSpinBox->value());
        emit updateClock();
    });

    connect(ui->positionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](){
        settings->setValue("position", static_cast<ClockPosition>(ui->positionComboBox->currentIndex()));
        emit updateClock();
    });

    connect(ui->timeFormatLineEdit, &QLineEdit::textChanged, this, [=](){
        settings->setValue("timeFormat", ui->timeFormatLineEdit->text());
        emit updateClock();
    });

    connect(ui->dateFormatLineEdit, &QLineEdit::textChanged, this, [=](){
        settings->setValue("dateFormat", ui->dateFormatLineEdit->text());
        emit updateClock();
    });

    connect(ui->strokeThicknessSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](){
        settings->setValue("strokeThickness", ui->strokeThicknessSpinBox->value());
        emit updateClock();
    });

    connect(ui->verticalPaddingSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](){
        settings->setValue("verticalPadding", ui->verticalPaddingSpinBox->value());
        emit updateClock();
    });

    connect(ui->horizontalPaddingSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](){
        settings->setValue("horizontalPadding", ui->horizontalPaddingSpinBox->value());
        emit updateClock();
    });
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::cancel()
{
    const QStringList keys = settingsHash.keys();

    Q_FOREACH(QString key, keys) {
        settings->setValue(key, settingsHash[key]);
    }

    emit updateClock();

    reject();
}
