#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QScreen>
#include <QSettings>
#include <QMessageBox>
#include <QFontDialog>
#include <QColorDialog>
#include "clock_position.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr, QHash<QString, QVariant> *settings = nullptr);
    ~SettingsDialog();

signals:
    void updateClock();
    void updateBlacklist();

private:
    Ui::SettingsDialog *ui;
    QHash<QString, QVariant> *settings;

    template <typename SettingValue>
    void updateSetting(QString settingName, SettingValue value);

    void connectSignals();
    void initializeSettingsDialog();
    void setFontPickerFont(QPushButton *fontPicker, QFont font);
    void setColorPickerPalette(QPushButton *colorPicker, QColor color);
    void changeFontSetting(QPushButton *fontPicker, QString fontSettingName);
    void changeColorSetting(QPushButton *colorPicker, QString colorSettingName);

    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void save();
    void cancel();
};

#endif // SETTINGSDIALOG_H
