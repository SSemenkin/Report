#ifndef INFLUXDATA_H
#define INFLUXDATA_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>

class InfluxData : public QObject
{
    Q_OBJECT
public:
    explicit InfluxData(QObject *parent = nullptr);

    void setCellList(const QStringList cellList) {
        list = cellList;
    }
    QStringList getCellList() const {
        return list;
    }

    void getKPIData(const QString influxQuery);
signals:
    void dataReady();
    void errorCatched(const QString errorText);
    void deleteMe();

private slots:
    void recieveData(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;

    QMap<QString,QMap<QString,QMap<QDateTime,float>>> kpi_data;

    QStringList list;

    uint sended  = 0;
    uint recieve = 0;

};

#endif // INFLUXDATA_H
