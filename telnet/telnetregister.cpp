
#include "telnetregister.h"

TelnetRegister::TelnetRegister(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    timer = new QTimer(this);
    connect(socket,&QTcpSocket::readyRead,this,&TelnetRegister::processAnswersFromHLR);
    timer->start(15000);
    connect(timer,&QTimer::timeout,[=](){
        timer->stop();
        timer->deleteLater();
        socket->disconnectFromHost();
        loging(allOperationText);
        emit errorCatched("Timeout(10s).");
        emit executed();
    });
}

void TelnetRegister::reRegisterAbonent(const QString abonentNumber)
{
    validateNumber(abonentNumber);
    __behavior = OperationBehavior::Reregister;
    loging();
    socket->connectToHost(ipHLR,23);
}

void TelnetRegister::printProfileAbonent(const QString abonentNumber)
{
    validateNumber(abonentNumber);
    __behavior = OperationBehavior::PrintProfile;
    loging();
    socket->connectToHost(ipHLR,23);
}

void TelnetRegister::changePDPCP(const QString abonentNumber)
{
    validateNumber(abonentNumber);
    __behavior = OperationBehavior::ChangeSUD;
    loging();
    socket->connectToHost(ipHLR,23);
}

void TelnetRegister::processAnswersFromHLR()
{



    QString reply = socket->readAll().toLower();
    allOperationText += reply;
    ///AUTH
    if(reply.right(12) == "login name: "){
        socket->write(QString(loginName+"\r\n").toUtf8());
    } else if(reply.right(10) == "password: "){
        socket->write(QString(password + "\r\n").toUtf8());
    } else if(reply.right(8) == "domain: "){
        socket->write(QString("\r\n").toUtf8());
    } else if(!subString(reply,"winnt\\profiles\\").isEmpty()){
        socket->write(QString("mml -a\r\n").toUtf8());
    } else if(!subString(reply,"hlr1").isEmpty()) {
        socket->write(QString("hgsdp:msisdn = "+__abonent+",all;\r\n").toUtf8());
    }
    ///AUTH END
    /// BEGIN BEHAVIOR BRANCHING
    if(__behavior == OperationBehavior::ChangeSUD){
        if(!subString(reply,"end").isEmpty()){
           __pdpcp = subString(reply,"pdpcp-").left(7).right(1) == "1" ? "3" : "1";
           socket->write(QString("hgsdc:msisdn = "+__abonent+",sud=pdpcp-"+__pdpcp+";\r\n").toUtf8());
        }
        else if(!subString(reply,"ecuted").isEmpty() && !countCommands){
           countCommands ++;
           timer->stop();
           socket->disconnectFromHost();
           emit successed();
           emit executed();
        }

    } else if(__behavior == OperationBehavior::Reregister){
        if(!subString(reply,"end").isEmpty()){
           __imsi = subString(reply,"25599500").left(16);
           __pdpcp = subString(reply,"pdpcp-").left(7).right(1);
           __caw = subString(reply,"caw-").left(5).right(1);
           socket->write(QString("hgsue:msisdn = "+__abonent+";\r\n").toUtf8());
        }
        else if(!subString(reply,"ecuted").isEmpty() && countCommands == 0){
           countCommands ++;
           QString profile = __pdpcp == "1" ? "1" : "2";
           socket->write(QString("HGSUI:IMSI="+__imsi+",MSISDN= "+__abonent+" ,PROFILE="+profile+";\r\n").toUtf8());
        }
        else if(!subString(reply,"ecuted").isEmpty() && countCommands == 1){
            countCommands ++;
            socket->write(QString("hgsdc:msisdn = "+__abonent+",sud=caw-"+__caw+";\r\n").toUtf8());
        }
        else if(!subString(reply,"ecuted").isEmpty() && countCommands == 2){
            countCommands++;
            timer->stop();
            socket->disconnectFromHost();
            emit successed();
            emit executed();
        }
    } else {
        if(!subString(reply,"end").isEmpty()){
            timer->stop();
            socket->disconnectFromHost();
            emit profileReady(reply);
            emit executed();
        }
    }

    ///ERRORS
    if(!subString(reply,"fault code").isEmpty()){
        timer->stop();
        socket->disconnectFromHost();
        loging(allOperationText);
        emit errorCatched(reply);
        emit executed();
    }
}

void TelnetRegister::validateNumber(const QString number)
{
    if(number.left(2) == "72") {
         __abonent = "380" + number;
    } else if(number.left(3) == "072"){
        __abonent = "38" + number;
    } else {
        __abonent = number;
    }
}

QString TelnetRegister::subString(const QString text, const QString buffer)
{
    for(int i=0;i<text.length()-buffer.length();i++){
        QString tmp = text.right(text.length()-i).left(buffer.length());
        if(tmp == buffer){
            return text.right(text.length()-i);
        }
    }
    return "";
}

void TelnetRegister::loging(QString errorText) const
{
    if(errorText.isEmpty()){
        QFile file("telnet-operations.txt");
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QString operation = __behavior == OperationBehavior::ChangeSUD ? " Смена тарифа " : __behavior == OperationBehavior::Reregister ?
                                                                             " Перерегистрация " : " Принт профайла ";
        file.write(QString(QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate) +operation +__abonent+"\n").toUtf8());
        file.close();
    } else {
        QFile file("errorsTelnet-operations.txt");
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QString operation = __behavior == OperationBehavior::ChangeSUD ? " Смена тарифа " : __behavior == OperationBehavior::Reregister ?
                                                                             " Перерегистрация " : " Принт профайла ";
        file.write(QString(QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate) +operation +__abonent+"\n ErrorText:"+errorText+"\n\n").toUtf8());
        file.close();
    }
}
