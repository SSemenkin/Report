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


    connect(this, &CustomSlice::hovered, this, &CustomSlice::showHighlight);
    connect(this, &CustomSlice::clicked, this, &CustomSlice::getData);

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
        brush.setColor(brush.color().darker());
        setExploded();
        setBrush(brush);
        emit sliceHovered(label());
    } else {
        setBrush(m_originalBrush);
        setExploded(false);
        emit sliceUnHovered();
    }
}

void CustomSlice::getData()
{
    if(label().left(1)!='(') {
        TelnetSeparateCell *telnet = new TelnetSeparateCell;
        connect( telnet, &TelnetSeparateCell::errorCatched, [=] (const QString errorText){
            QMessageBox::critical( nullptr,"Ошибка операции",errorText);
        });
        connect( telnet, &TelnetSeparateCell::missSettings, [=] (){
            QMessageBox::critical( nullptr,"Ошибка операции","Отсутствуют настройки подключения к контроллеру.\n"
                                                             "Пожалуйста заполните недостающие поля.");
        });
        connect( telnet, &TelnetSeparateCell::timeout, [=] (){
            QMessageBox::critical( nullptr,"Ошибка операции","Превышено время ожидания (timeout 15 sec).");
        });
        connect( telnet, &TelnetSeparateCell::deleteMe, telnet, &TelnetSeparateCell::deleteLater);
        connect( telnet, &TelnetSeparateCell::success, [=](const QString cell){
            LoadStateCheck *widget = new LoadStateCheck(cell);
            widget->show();

        });
        telnet->getData(TelnetSeparateCell::Operation::PrintSeparateCell,label());
    } else QMessageBox::information(nullptr,"Неудачная операция", "Получение данных сети 3g для нас пока является невыполнимым заданием.\n"
                                                                  "Может быть в дальнейшем наша команда сможет и это.");
}




#include "moc_customslice.cpp"
