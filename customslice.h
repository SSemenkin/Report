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
#ifndef CUSTOMSLICE_H
#define CUSTOMSLICE_H

#include <QtCharts/QPieSlice>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QProcess>
#include <QTcpSocket>



#include "loadstatecheck.h"
#include "settings.h"

QT_CHARTS_USE_NAMESPACE

class CustomSlice : public QPieSlice
{
    Q_OBJECT

public:
    CustomSlice(QString label, qreal value);

public:
    QBrush originalBrush();


    void readData();

    void writeFile(QString);

    bool subString(QString buff, QString findStr);

public Q_SLOTS:
    void showHighlight(bool show);

    void getData();

    void loadSettings();

signals:

    void sliceHovered(QString);

private:

    bool is_tg = false;

    QBrush m_originalBrush;

    QString cell,cell_print,current_tg,msp_print;

    LoadStateCheck *a;

    QTcpSocket *socket;

    bool is_cell_print = false;

    bool is_msp_print = false;

    QString IP,Login,Password;


};

#endif // CUSTOMSLICE_H
