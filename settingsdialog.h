#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include "clockPosition.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr, QSettings *settings = nullptr);
    ~SettingsDialog();

signals:
    void updateClock();

private:
    QSettings *settings;
    QHash<QString, QVariant> settingsHash;

    Ui::SettingsDialog *ui;

private slots:
    void cancel();
};

#endif // SETTINGSDIALOG_H
