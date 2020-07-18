#include "chartbyseparatethread.h"

ChartBySeparateThread::ChartBySeparateThread()
{
    settings = new Settings();
}

ChartBySeparateThread::~ChartBySeparateThread()
{
    delete settings;
}

bool ChartBySeparateThread::openLocalDataBase()
{
    mysqldataBase = QSqlDatabase::addDatabase("QMYSQL","QMYSQL_detail_connection");
    mysqldataBase.setPort(settings->getSettigns("mainForm/mySqlPort").toInt());
    mysqldataBase.setHostName(settings->getSettigns("mainForm/mySqlHost").toString());
    mysqldataBase.setDatabaseName(settings->getSettigns("mainForm/mysqlDatabaseName").toString());
    mysqldataBase.setUserName(settings->getSettigns("mainForm/mySqlUserName").toString());
    mysqldataBase.setPassword(settings->getSettigns("mainForm/mySqlPassword").toString());
    mysqldataBase.open();
    return mysqldataBase.isOpen();
}

QVector<cell2G> ChartBySeparateThread::getChargeOf2GCellsFromMySQL()
{
    QVector<cell2G> result(cells.size());
    if(days == 0)
    {
        result.clear();
        return result;
    }

    emit dataRecieved(0);
    if(openLocalDataBase())
    {
        QSqlQueryModel *chargeModel = new QSqlQueryModel;
        QString query;
        for(int i=0;i<cells.size();i++)
        {
            query = "select TCH - BLOC as TCH,SPEECH,GPRS,TIME,INTERFERENCE from bscrecords where RBS='"+cells[i].left(6)+"' and CELLNAME = '"+cells[i].right(1)+"' and TIME between '"+QDateTime::currentDateTime().addDays(-days).toString(Qt::ISODate)
                    +"' and '"+QDateTime::currentDateTime().toString(Qt::ISODate)+"' order by TIME";

            chargeModel->setQuery(query,mysqldataBase);

            cell2G tmp;
            tmp.cellName = cells[i];
            for(int j=0;j<chargeModel->rowCount();j++)
            {
                tmp.dates.push_back(QDateTime::fromString(chargeModel->data(chargeModel->index(j,3)).toString(),Qt::ISODate));
                if(chargeModel->index(j,0).data().toFloat() != 0.0) {
                    tmp.charges.push_back(((chargeModel->index(j,1).data().toFloat()+chargeModel->index(j,2).data().toFloat()) / chargeModel->index(j,0).data().toFloat())*100.0);
                } else tmp.charges.push_back(100);
                tmp.inter.push_back(chargeModel->index(j,4).data().toInt());
            }
            result[i] = tmp;
            emit dataRecieved((double(i)/double(cells.size())*100.0));
        }
        emit dataRecieved(100);
        delete chargeModel;
        mysqldataBase.close();

    } else {
         emit dataBaseDontOpen();
         return QVector<cell2G>();
    }
    returneD = result;

    emit finished();
    return result;
}
