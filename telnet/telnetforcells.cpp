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
    if(!subString(responce,"not accepted").isEmpty()){
        emit errorCatched(responce);
        emit executed();
        return;
    }
    if(!subString(responce, "fault code").isEmpty()){
        emit errorCatched(responce);
        emit executed();
        return;
    }
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
        } else if(subString(responce,"end").length() && subString(responce,"radio x-ceiver administration").length() &&!firstFlag){
            tg = subString(responce,"rxotg-").left(9).right(3).toInt();
            telnet->write(QString("rxtcp:mo=rxotg-"+QString::number(tg)+";\r\n").toUtf8());
            firstFlag = !firstFlag;
        } else if(subString(responce, "end").length() && subString(responce,"tg to channel group connection").length() && firstFlag){
            cellNames = parseCellList(responce);
            telnet->write(QString("rlcrp:cell = " + cellNames[currentCell]+";\r\n").toUtf8());

        }
    } else  {
        cellText +=responce;
        if(subString(responce,"end").length() && currentCell == 0) {
            prints << cellText;
            cellText.clear();

            currentCell++;
            if((int)currentCell < cellNames.size() )
            {
                telnet->write(QString("rlcrp:cell = " + cellNames[currentCell]+";\r\n").toUtf8());
            } else {
                emit dataReady(cellNames,prints);
                emit executed();
            }
        } else if(subString(responce,"end").length() && currentCell == 1){
            prints << cellText;
            cellText.clear();

            currentCell++;
            if((int)currentCell < cellNames.size() )
            {
                telnet->write(QString("rlcrp:cell = " + cellNames[currentCell]+";\r\n").toUtf8());
            } else  {
                emit dataReady(cellNames,prints);
                emit executed();
            }
        } else if(subString(responce,"end").length() && currentCell == 2)
        {
            prints << cellText;
            cellText.clear();
            emit dataReady(cellNames,prints);
            emit executed();
        }
    }

}

QString TelnetForCells::subString(const QString text, const QString buffer)
{
    for(int i=0;i<text.length()-buffer.length();i++){
        QString tmp = text.right(text.length()-i).left(buffer.length());
        if(tmp == buffer){
            return text.right(text.length()-i);
        }
    }
    return "";
}

QStringList TelnetForCells::parseCellList(QString answer)
{
    QStringList result;
    answer = subString(answer,"lu");
    result << answer.left(7);
    subString(answer.split("\r\n")[1],"lu");
    if(!subString(answer.split("\r\n")[1],"lu").isEmpty()) result << subString(answer.split("\r\n")[1],"lu").left(7);
    if(!subString(answer.split("\r\n")[2],"lu").isEmpty()) result << subString(answer.split("\r\n")[2],"lu").left(7);
    return result;
}
