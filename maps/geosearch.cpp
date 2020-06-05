#include "geosearch.h"

GeoSearch::GeoSearch(QObject *parent) : QObject(parent)
{
    read_rbs_choords();
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &GeoSearch::receiveData);
}

GeoSearch::~GeoSearch()
{

}

bool GeoSearch::search(QString adress)
{
    try {
        request.setUrl(make_request(adress));
        manager->get(request);
    } catch (...) {
        return false;
    }

    return true;
}

void GeoSearch::receiveData(QNetworkReply *data)
{
    try {
        QPair<long double, long double> point = getPoint(data->readAll());
        search_nearest_objects(point.first, point.second);
    } catch (...) {
        qDebug() << "Error reply";
    }
}

QUrl GeoSearch::make_request(QString adress)
{
    QString request_text =
            QString("https://geocode-maps.yandex.ru/1.x/?apikey=cac98c74-acf3-4406-9353-e398aea4c82a&geocode=" + adress);

    return QUrl(QUrl(request_text.toUtf8()).toEncoded());
}

long double GeoSearch::calc_distance(long double x1, long double y1, long double x2, long double y2)
{
    return sqrt(abs(pow(x1 - x2, 2)) + abs(pow(y1 - y2, 2))) * 100000;
}

QStringList GeoSearch::search_nearest_objects(long double x, long double y)
{
    QMap<QString, long double> map;
    QVector<QPair<long double, QString>> sorted_list;
    for (auto iter = rbs.begin(); iter != rbs.end(); ++iter){
        sorted_list.push_back(QPair<long double, QString>(calc_distance(
                       rbs[iter.key()].first, rbs[iter.key()].second, x, y), iter.key()));
    }

    std::sort(sorted_list.begin(), sorted_list.end());

    QStringList result;

    for(long double i = 0; i < 5 && i < sorted_list.size(); i++)
    {
        qDebug() << double(sorted_list[i].first) << " м." << sorted_list[i].second;
        result.append(sorted_list[i].second);
    }
    emit dataReady(result);
    return result;
}

QStringList GeoSearch::get_elements(QByteArray line)
{
    return QString(line).replace(',','.').split('\t',QString::SplitBehavior::SkipEmptyParts);
}

QPair<long double, long double> GeoSearch::getPoint(QByteArray data)
{
    QFile outp("output.txt");
    outp.open(QIODevice::WriteOnly);
    outp.write(data);
    QXmlStreamReader xmlDoc(data);
    while (!xmlDoc.atEnd() && !xmlDoc.hasError()){
        QXmlStreamReader::TokenType token = xmlDoc.readNext();
        if (token == QXmlStreamReader::StartElement){
            if (xmlDoc.name() == "pos"){
                QString line = QString(xmlDoc.readElementText().toUtf8());
                long double x = line.split(' ', QString::SplitBehavior::SkipEmptyParts)[0].toDouble();
                long double y = line.split(' ', QString::SplitBehavior::SkipEmptyParts)[1].toDouble();

                return QPair<long double, long double>(x, y);

            }
        }
    }
    return QPair<long double, long double>(0, 0);
}

void GeoSearch::read_rbs_choords()
{
    if(rbs.size()){
        rbs.clear();
    }
    QFile file("rbs.txt");
    file.open(QIODevice::ReadOnly);
    try {

        while (!file.atEnd()){
            QStringList line = get_elements(file.readLine());
            rbs[line[0]] = QPair<long double,long double>(line[1].toDouble(),line[2].toDouble());
        }
    } catch (...) {
        QMessageBox::warning(0,"Warning","Файл с координатами объектов не найден");
    }
}
