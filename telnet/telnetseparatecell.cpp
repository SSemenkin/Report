#include "telnetseparatecell.h"

TelnetSeparateCell::TelnetSeparateCell(QObject *parent) : QObject(parent)
{
    telnet = new QTcpSocket();
    settings = new Settings();
    QObject::connect(telnet, &QTcpSocket::readyRead, this, &TelnetSeparateCell::readData);
}

TelnetSeparateCell::~TelnetSeparateCell()
{
    delete telnet;
    delete settings;
}

void TelnetSeparateCell::getData(TelnetSeparateCell::Operation choice,const QString cellName)
{
    if(isBSC03CELL(cellName)){
        ip = "172.25.157.99";
        userName = "administrator";
        password = "Administrator1@";
    } else {
        ip = settings->getIP();
        userName = settings->getLogin();
        password = settings->getPassword();
    }
    if(ip.isEmpty() || userName.isEmpty() || password.isEmpty()) {
        delete settings;
        emit missSettings();
        emit deleteMe();
        return;
    }
    state = choice;
    cell = cellName;
    telnet->connectToHost(ip,23);
    QObject::connect(&timer,  &QTimer::timeout, [=](){
            telnet->disconnectFromHost();
            timer.stop();
            emit timeout(cellName);
            emit deleteMe();
    });

    timer.start(15000);
}

void TelnetSeparateCell::readData()
{
    QString answer = telnet->readAll();
    allRecievedData += answer;
    switch (state)
    {
        case Default:
            proccesingSeparateCell(answer);
            break;
        case PrintSeparateCell:
            proccesingSeparateCell(answer);
            break;
        default:
            proccesingSeparateCell(answer);
            break;
    }
}

void TelnetSeparateCell::proccesingSeparateCell(QString text)
{
    flag ? responce += text : responce +="";
    qDebug() << text;
    if(subString(text,"login name:")){
        telnet->write(QString(userName + "\r\n").toUtf8());
    } else if(subString(text, "password:")){
        telnet->write(QString(password + "\r\n").toUtf8());
    } else if(subString(text, "domain")) {
        telnet->write(QString("\r\n").toUtf8());
    } else if(subString(text, "WINNT\\Profiles\\")) {
        telnet->write(QString("mml -a\r\n").toUtf8());
    } else if(subString(text,"BSC01LUG") || subString(text,"BSC3LUG")) {
        flag = !flag;
        telnet->write(QString("rlcrp:cell = "+cell+";\r\n").toUtf8());
    } else if(subString(responce,"end") && subString(responce,"CELL RESOURCES")) {
        moveToFile(responce,cell+".txt");
        responce.clear();
        telnet->write(QString("rxtcp:moty=rxotg,cell="+cell+";\r\n").toUtf8());
    } else if(subString(responce, "end") && subString(responce,"TG TO CHANNEL GROUP CONNECTION DATA")){
        for(int i=0;i<responce.size()-5;i++){
            if(responce.right(responce.length()-i).left(6) == "RXOTG-"){
                _tg = responce.right(responce.length()-i).left(9).right(3);
                break;
            }
        }
        responce.clear();
        telnet->write(QString("rxmsp:mo=rxotg-"+_tg+", subord;\r\n").toUtf8());
    } else if(subString(responce,"end") && subString(responce,"MANAGED OBJECT STATUS")){
        telnet->disconnectFromHost();
        moveToFile(responce,cell+"-msp.txt");
        responce.clear();
        emit success(cell);
        emit deleteMe();
    } else if(subString(responce, "NOT ACCEPTED") || subString(responce, "error") || subString(responce,"format error")){
        telnet->disconnectFromHost();
        emit errorCatched(text);
        emit deleteMe();
    }
}

bool TelnetSeparateCell::subString(QString buff, QString findStr) const
{
    buff = buff.toLower();
    findStr = findStr.toLower();
    for(int i=1;i<buff.length()-(findStr.length()-1);i++)
    {
        QString substring;
        for(int j=0;j<findStr.length();j++)
            substring+=buff[i+j];
        if(substring==findStr) return true;
    }
    return false;
}

void TelnetSeparateCell::moveToFile(const QString content, const QString filePath) const
{
    QFile file(filePath);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    file.write(content.toUtf8());
    file.close();
}

bool TelnetSeparateCell::isBSC03CELL(const QString cellName)
{
    QStringList cells = cellListBSC03.split(',');
    for(int i=0;i<cells.size();i++){
        if(cells[i] == cellName) return true;
    }
    return false;
}
