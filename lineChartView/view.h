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

#ifndef VIEW_H
#define VIEW_H
#include <QtWidgets/QGraphicsView>
#include <QtCharts/QChartGlobal>
#include <QDateTime>
#include <QtCharts/QLegendMarker>
#include <QtCharts>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QMouseEvent;
class QResizeEvent;
class QWheelEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

class Callout;

QT_CHARTS_USE_NAMESPACE

class View: public QChartView
{
    Q_OBJECT

public:
    View(QMap<QString,QList<QStringList>> data,int index,QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event) ;
    void wheelEvent(QWheelEvent *event)
        {
            qreal factor = event->angleDelta().y() > 0 ? 2.0: 0.5;
            chart()->zoom(factor);
            event->accept();
            QChartView::wheelEvent(event);
        }
public slots:
    void keepCallout();
    void tooltip(QPointF point, bool state);

private:
    void markerClickHandle();
    void sort(QMap<QString, QList<QStringList> > &data);
    QChart *m_chart;
    Callout *m_tooltip;
    QList<Callout *> m_callouts;
};

#endif
