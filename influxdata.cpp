#include "influxdata.h"

InfluxData::InfluxData(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    QObject::connect( manager , &QNetworkAccessManager::finished , this, &InfluxData::recieveData);
}

void InfluxData::getKPIData(const QString influxQuery)
{
    if(list.isEmpty()) {
        emit errorCatched("cell list is empty...");
        emit deleteMe();
        return;
    }

    for(int i=0;i<list.size();i++){
        QString tmpQuery = influxQuery;
        tmpQuery.replace("CELL",list[i]);
        manager->get(QNetworkRequest(QUrl(tmpQuery)));
        sended++;
    }
}

void InfluxData::recieveData(QNetworkReply *reply)
{
    recieve++;
    if(reply->error()) {
        emit errorCatched(reply->errorString());
        emit deleteMe();
        return;
    }


}
