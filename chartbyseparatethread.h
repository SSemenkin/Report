#ifndef CHARTBYSEPARATETHREAD_H
#define CHARTBYSEPARATETHREAD_H

#include <QObject>
#include <QWidget>
#include <QTabWidget>
#include <QtCharts>
#include <QChart>
#include <QVector>
#include <QtSql>

#include "settings.h"

QT_CHARTS_USE_NAMESPACE

struct cell2G
{
    QString cellName;
    QVector<float> charges;
    QVector<QDateTime> dates;
    QVector<int> inter;
};

class ChartBySeparateThread : public QObject
{
    Q_OBJECT
public:
    ChartBySeparateThread();
    ~ChartBySeparateThread();


    QVector<cell2G> getChargeOf2GCellsFromMySQL();

    QVector<QString> cells;

    QVector<cell2G> returneD;

    int days;

signals:

    void finished();

private:

    bool openLocalDataBase();


    QSqlDatabase mysqldataBase;

    Settings *settings;




};

#endif // CHARTBYSEPARATETHREAD_H
