#include "telnetforcells.h"

TelnetForCells::TelnetForCells(QObject *parent) : QObject(parent)
{
    telnet = new QTcpSocket(this);
    settings = new Settings();
    ipBSC = settings->getIP();
    userNameBSC = settings->getLogin();
    passwordBSC = settings->getPassword();
    connect (telnet, &QTcpSocket::readyRead, this, &TelnetForCells::readDataFromController);
}
TelnetForCells::~TelnetForCells()
{
    delete settings;
}

void TelnetForCells::getLoadPerCell(const QString rsite)
{
    rbsName = rsite;
    telnet->connectToHost(ipBSC,23);
}

void TelnetForCells::readDataFromController()
{
    QString responce = telnet->readAll().toLower();
    qDebug() << responce;
    if(!cellNames.size()) {
        if(subString(responce,"ogin name:").length()){
            telnet->write(QString(userNameBSC+"\r\n").toUtf8());
        } else if(subString(responce,"password:").length()){
            telnet->write(QString(passwordBSC+"\r\n").toUtf8());
        } else if(subString(responce,"domain:").length()) {
            telnet->write("\r\n");
        } else if(subString(responce, "winnt\\profiles\\smena").length()){
            telnet->write("mml -a\r\n");
        } else if(subString(responce, "bsc").length()){
            telnet->write(QString("rxtcp:moty=rxotg,cell="+rbsName+"a;\r\n").toUtf8());
        } else if(subString(responce,"end").length() && subString(responce,"xtcp").length()){
            tg = subString(responce,"xotg-").left(8).right(3).toInt();
            telnet->write(QString("rxtcp:mo=rxotg-"+QString::number(tg)+";\r\n").toUtf8());
        } else if(subString(responce, "end").length() && subString(responce,"tg to channel group connection").length()){
            cellNames << subString(responce,"lu").left(7);
            if(!subString(cellNames[0],"lu").left(7).isEmpty()){
                cellNames << subString(cellNames[0],"lu").left(7);
                if(!subString(cellNames[1],"lu").left(7).isEmpty())
                {
                    cellNames << subString(cellNames[1],"lu").left(7);
                }
            }
            qDebug() << "CELLNAMES" << cellNames;
            telnet->write(QString("rlcrp:cell = " + cellNames[currentCell]+",detail;\r\n").toUtf8());
            forSignal << cellNames[currentCell];
        }
    } else  {
        cellText +=responce;
        if(subString(responce,"end").length() && currentCell == 0) {
            QFile file(cellNames[currentCell]+".txt");
            file.open(QIODevice::WriteOnly);
            file.write(cellText.toUtf8());
            file.close();
            cellText.clear();

            currentCell++;
            if(currentCell < cellNames.size() )
            {
                telnet->write(QString("rlcrp:cell = " + cellNames[currentCell]+",detail;\r\n").toUtf8());
                forSignal << cellNames[currentCell];
            } else {
                emit dataReady(forSignal);
                emit executed();
            }
        } else if(subString(responce,"end").length() && currentCell == 1){
            QFile file(cellNames[currentCell]+".txt");
            file.open(QIODevice::WriteOnly);
            file.write(cellText.toUtf8());
            file.close();
            cellText.clear();

            currentCell++;
            if(currentCell < cellNames.size() )
            {
                telnet->write(QString("rlcrp:cell = " + cellNames[currentCell]+",detail;\r\n").toUtf8());
                forSignal << cellNames[currentCell];
            } else  {
                emit dataReady(forSignal);
                emit executed();
            }
        } else if(subString(responce,"end").length() && currentCell == 2)
        {
            QFile file(cellNames[currentCell]+".txt");
            file.open(QIODevice::WriteOnly);
            file.write(cellText.toUtf8());
            file.close();
            cellText.clear();
            emit dataReady(forSignal);
            emit executed();
        }
    }

}

QString TelnetForCells::subString(const QString text, const QString buffer)
{
    for(int i=0;i<text.length()-buffer.length();i++){
        QString tmp = text.right(text.length()-1-i).left(buffer.length());
        if(tmp == buffer){
            return text.right(text.length()-1-i);
        }
    }
    return "";
}
