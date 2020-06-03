#ifndef GEOSEARCH_H
#define GEOSEARCH_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QMap>
#include <QFile>
#include <QMessageBox>
#include <math.h>
#include <QXmlStreamReader>
#include <QVector>



class GeoSearch : public QObject
{
    Q_OBJECT
public:
    explicit GeoSearch(QObject *parent = nullptr);
    ~GeoSearch();
    bool search(QString);
    //void setRange(int km);
    //void setCount(int rbs);

private slots:
    void receiveData(QNetworkReply*);

private:
    QUrl make_request(QString);
    long double calc_distance(long double x1, long double y1, long double x2, long double y2);
    QStringList search_nearest_objects(long double x, long double y);
    void read_rbs_choords();
    QStringList get_elements(QByteArray);
    QPair<long double, long double> getPoint(QByteArray);

    QNetworkRequest request;
    QNetworkAccessManager *manager;
    QStringList rbs_list;
    QMap<QString,QPair<long double,long double>> rbs;


signals:
    void dataReady(QStringList);
};

#endif // GEOSEARCH_H
