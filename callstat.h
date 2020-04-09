#ifndef CALLSTAT_H
#define CALLSTAT_H

#include <QDialog>
#include <QtSql>
#include <QString>
#include <QMessageBox>
#include <QTableView>


#include "settings.h"

namespace Ui {
class CallStat;
}

class CallStat : public QDialog
{
    Q_OBJECT

public:
    explicit CallStat(QWidget *parent = nullptr);
    ~CallStat();

    void onStart();

    QString getQuery();

    void setDriver(QString);

private slots:
    void on_pushButton_clicked();


private:
    Ui::CallStat *ui;

    QSqlDatabase dataBase;


    QString driver;

    Settings *settings;

};

#endif // CALLSTAT_H
