#ifndef CALLSTAT_H
#define CALLSTAT_H

#include <QDialog>
#include <QtSql>
#include <QString>
#include <QMessageBox>
#include <QTableView>



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

private slots:
    void on_pushButton_clicked();


private:
    Ui::CallStat *ui;

    QSqlDatabase dataBase;

    QString dataBaseName = "cdrmss1lug",
            dataBaseHost = "193.228.160.5",
            dataBaseUser = "ebondarenko",
            dataBasePassword ="sQtgkuUTP9j8m0XF";

    const int dataBasePort = 3306;

};

#endif // CALLSTAT_H
