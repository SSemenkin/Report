#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QMessageBox>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();


    QString getPassword() const;

    QString getLogin() const;

    QString getIP() const ;

    QString getSettigns(QString key) const;

    void saveAuthData(QString key,QString value) const;

    void saveSettings();

    void loadSettings();

private:
    Ui::Settings *ui;

    QString Login;
    QString Password;
    QString IP;

    QSettings *settings;
};

#endif // SETTINGS_H
