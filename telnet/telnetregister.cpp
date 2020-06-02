
#include "telnetregister.h"

TelnetRegister::TelnetRegister(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    timer = new QTimer(this);
    connect(socket,&QTcpSocket::readyRead,this,&TelnetRegister::processAnswersFromHLR);
}

void TelnetRegister::reRegisterAbonent(const QString abonentNumber)
{
    if(abonentNumber.left(2) == "72") {
         __abonent = "380" + abonentNumber;
    } else if(abonentNumber.left(3) == "072"){
        __abonent = "38" + abonentNumber;
    } else {
        __abonent = abonentNumber;
    }
    socket->connectToHost(ipHLR,23);
}

void TelnetRegister::printProfileAbonent(const QString abonentNumber)
{
    is_profile_print = true;
    reRegisterAbonent(abonentNumber);
}

void TelnetRegister::processAnswersFromHLR()
{
    timer->setInterval(10000);
    timer->start();
    connect(timer,&QTimer::timeout,[=](){
        timer->stop();
        socket->disconnectFromHost();
        emit errorCatched("Timeout(10s).");
        emit executed();
    });
    QString reply = socket->readAll().toLower();
    if(reply.right(12) == "login name: "){
        socket->write(QString(loginName+"\r\n").toUtf8());
    }
    else if(reply.right(10) == "password: "){
        socket->write(QString(password + "\r\n").toUtf8());
    }
    else if(reply.right(8) == "domain: "){
        socket->write(QString("\r\n").toUtf8());
    }
    else if(!subString(reply,"winnt\\profiles\\smena").isEmpty()){
        socket->write(QString("mml -a\r\n").toUtf8());
    }
    else if(!subString(reply,"hlr1").isEmpty()){
        socket->write(QString("hgsdp:msisdn = "+__abonent+",all;\r\n").toUtf8());
    }
    else if(!subString(reply,"end").isEmpty() && !is_profile_print){
        __imsi = subString(reply,"255995000").left(15);
        __pdpcp = subString(reply,"pdpcp").left(7).right(1);
        socket->write(QString("hgsue:msisdn = "+__abonent+";\r\n").toUtf8());
    }
    else if(!subString(reply,"end").isEmpty() && is_profile_print){
        socket->disconnectFromHost();
        timer->stop();
        emit profileReady(reply);
        emit executed();
    }
    else if(!subString(reply,"ecuted").isEmpty() && !firstFlag){
        if(__pdpcp != "1"){
            __pdpcp = "2";
        }
        firstFlag = true;
        socket->write(QString("HGSUI:IMSI="+__imsi+" ,MSISDN= "+__abonent+" ,PROFILE="+__pdpcp+";\r\n").toUtf8());

    }
    else if(!subString(reply,"ecuted").isEmpty() && firstFlag){
        socket->disconnectFromHost();
        timer->stop();
        emit successed();
        emit executed();
    }
    else if(!subString(reply,"fault code").isEmpty()){
        socket->disconnectFromHost();
        timer->stop();
        emit errorCatched(subString(reply,"fault code"));
        emit executed();
    }

}

QString TelnetRegister::subString(const QString text, const QString buffer)
{
    for(int i=0;i<text.length()-buffer.length();i++){
        QString tmp = text.right(text.length()-1-i).left(buffer.length());
        if(tmp == buffer){
            return text.right(text.length()-1-i);
        }
    }
    return "";
}
