/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "customslice.h"

QT_CHARTS_USE_NAMESPACE

CustomSlice::CustomSlice(QString label, qreal value)
    : QPieSlice(label, value)
{   

    loadSettings();

    setLabelPosition(LabelInsideNormal);

    int red = rand()%256;
    int green = rand()%256;
    int blue = rand()%256;
    if(red + green + blue > 700)
    {
        red -= 50;
        blue -=50;
        green -=50;
    }
    setBrush(QColor(red,green,blue));
    setLabelArmLengthFactor(0);

    socket = new QTcpSocket;
    connect(this, &CustomSlice::hovered, this, &CustomSlice::showHighlight);
    connect(this,&CustomSlice::clicked,this,&CustomSlice::getData);
    connect(socket,&QTcpSocket::readyRead,this,&CustomSlice::readData);


}

QBrush CustomSlice::originalBrush()
{
    return m_originalBrush;
}

void CustomSlice::showHighlight(bool show)
{
    if (show) {
        QBrush brush = this->brush();
        m_originalBrush = brush;
        brush.setColor(QColor(0, 250, 154));
        setExploded();
        setBrush(brush);
        emit sliceHovered(label());
    } else {
        setBrush(m_originalBrush);
        setExploded(false);   
    }
}

void CustomSlice::getData()
{
    if(IP.isEmpty() || Password.isEmpty() || Login.isEmpty())
    {
        QMessageBox::warning(nullptr,"Warning","У приложения не получится подключиться к контроллеру,пожалуйста,заполните данные для аутентификации.\n"
                                               "Они расположены в меню выше графика с частотным анализом.\n♥ ");
        return;
    }
    cell.clear();
    if(this->label().toStdString()[0]=='L')
    {
        std::string  s = this->label().toStdString();
        for(int i=0;i<7;i++)
            cell += s[i];
    }
    else
    {
        QMessageBox::information(nullptr,"Информация","К сожалению нашей команде не удалось\nреализовать данную функцию для сети 3G\n‿︵‿ヽ(°□° )ノ︵‿︵\nПриносим свои извинения");
        return;
    }

    socket->disconnectFromHost();
    is_tg = false;
    socket->connectToHost(IP,23);

}

void CustomSlice::loadSettings()
{
    Settings *settings = new Settings();
    IP = settings->getIP();
    Login = settings->getLogin();
    Password  = settings->getPassword();
    delete settings;
}

void CustomSlice::readData() //Telnet Connection
{

    QString all = socket->readAll();
    //qDebug() << all;
    if(subString(all,"login"))
    {
        QString command = Login+"\r\n";
        socket->write(command.toLatin1());
    }
    else if(subString(all,"NOT ACCEPTED") || subString(all,"FUNCTION"))
    {
        LoadStateCheck *ex = new LoadStateCheck("tmp");
        ex->setWindowFlag(Qt::WindowStaysOnTopHint);
        QMessageBox::warning(ex,"Информация","Функция занята");
        socket->disconnectFromHost();
        return;
    }
    else if(subString(all,"password: "))
    {
        QString command = Password+"\r\n";
        socket->write(command.toLatin1());
    }
    else if(subString(all,"NT Domain:"))
    {
        QString command = "\r\n";
        socket->write(command.toLatin1());
    }
    else if(subString(all,"WINNT\\Profiles\\smena"))
    {
        QString command = "mml -a\r\n";
        socket->write(command.toLatin1());
    }
    else if(all.left(2)=="WO" || all.left(4) == "EX-A")
    {
        QString command = "rlcrp:cell="+cell+";\r\n";
        socket->write(command.toLatin1());
        is_cell_print = true;
        cell_print.clear();
    }
    else if(subString(all,"TO CHANNEL GROUP CONNECTION"))
    {
        current_tg.clear();
        for(int i=0;i<all.length()-4;i++)
        {
            if(all[i]=="X" && all[i+1]=="O" && all[i+2]=="T" && all[i+3]=="G" && !is_tg)
                for(int j=i+5;j<all.length();j++)
                {
                    if(all[j]==' ')
                    {
                        is_tg = true;
                        break;
                    }
                    current_tg+=all[j];
                }
        }
        QString command = "rxmsp:mo=rxocf-"+current_tg+",subord;\r\n";
        socket->write(command.toLatin1());
    }
    else if(subString(all,"MANAGED OBJECT STATUS"))
    {
        is_msp_print = true;
        msp_print.clear();
        msp_print+=all;
    }
    else if(is_msp_print)
    {
        msp_print+=all;
        if(subString(all,"END") || subString(all,"u0003<") || all.right(6) == "u0003<")
        {
            QFile file;
            file.setFileName(cell+"-msp.txt");
            if(file.exists()) file.remove();
            file.open(QIODevice::WriteOnly | QIODevice::Truncate);
            file.write(msp_print.toLatin1());
            file.close();

            is_msp_print = false;

            socket->disconnectFromHost();
            a = new LoadStateCheck(cell);
            a->show();
        }
    }
    else if(is_cell_print)
    {
            cell_print+=all;
            if(subString(all,"END"))
            {
                writeFile(cell_print);
                QString command;
                command = "rxtcp:moty=rxotg,cell="+cell+";\r\n";
                socket->write(command.toLatin1());
                is_cell_print = false;

            }
    }
}

void CustomSlice::writeFile(QString print)
{
    QFile file;
    file.setFileName(cell+".txt");
    if(file.exists()) file.remove();
    file.open(QIODevice::WriteOnly  | QIODevice::Truncate);
    file.write(print.toLatin1());
    file.close();
}

bool CustomSlice::subString(QString buff, QString findStr)
{
    for(int i=1;i<buff.length()-(findStr.length()-1);i++)
    {
        QString substring;
        for(int j=0;j<findStr.length();j++)
            substring+=buff[i+j];
        if(substring==findStr) return true;
    }
    return false;
}

#include "moc_customslice.cpp"
