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

#include "view.h"
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtWidgets/QGraphicsTextItem>
#include "callout.h"
#include <QtGui/QMouseEvent>

View::View(QMap<QString,QList<QStringList>> data,int index,QWidget *parent)
    : QChartView(new QChart, parent),
      m_chart(0),
      m_tooltip(0)
{
    sort(data);
    m_chart = new QChart;
    m_chart->setAnimationOptions(QChart::AnimationOption::AllAnimations);
    QValueAxis *axisY  = new QValueAxis;
    !index ? axisY->setRange(0,100):
                   axisY->setRange(0,5);
    QDateTimeAxis *axisX = new QDateTimeAxis;

    QCategoryAxis *catAxis;
    catAxis = new QCategoryAxis;
    if(index == 1) {
        axisY->setTickCount(6);
        catAxis->append("Норма",1.5);
        catAxis->append("Повышенное значение",5);

    }
    catAxis->setLinePen(QPen(Qt::red));
    catAxis->setGridLinePen(QPen(Qt::red));
    catAxis->setLinePenColor(Qt::red);
    m_chart->addAxis(catAxis, Qt::AlignRight);

    m_chart->addAxis(axisY,Qt::AlignLeft);
    m_chart->addAxis(axisX,Qt::AlignBottom);
    for( auto target = data.begin(); target != data.end(); ++target) {
        QLineSeries *series = new QLineSeries;
        series->setPointsVisible();
        series->setName(target.key());


        connect(series, &QLineSeries::hovered, this, &View::tooltip);

        auto container = target.value();
        for(int x = 0; x < container.size();x++){
                series->append(QDateTime::fromString(container.at(x)[2],Qt::DateFormat::ISODate).toMSecsSinceEpoch(),container.at(x)[index].toDouble());
        }
        m_chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        if(index == 1) series->attachAxis(catAxis);
    }
    m_chart->legend()->setAlignment(Qt::AlignRight);
    m_chart->setAcceptHoverEvents(true);

    setRenderHint(QPainter::Antialiasing);
    setChart(m_chart);


    this->setMouseTracking(true);
    const auto markers = m_chart->legend()->markers();
    for(int i=0;i<markers.count();i++){
        QObject::connect( markers.at(i), &QLegendMarker::clicked, this, &View::markerClickHandle);
    }
    setRubberBand(QChartView::RubberBand::HorizontalRubberBand);
}

void View::resizeEvent(QResizeEvent *event)
{
     const auto callouts = m_callouts;
     for (Callout *callout : callouts)
          callout->updateGeometry();
    QChartView::resizeEvent(event);
}

void View::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        m_chart->zoomIn();
        break;
    case Qt::Key_Minus:
        m_chart->zoomOut();
        break;
    case Qt::Key_Left:
        m_chart->scroll(-5.0, 0);
        break;
    case Qt::Key_Right:
        m_chart->scroll(5.0, 0);
        break;
    case Qt::Key_Up:
        m_chart->scroll(0, 5.0);
        break;
    case Qt::Key_Down:
        m_chart->scroll(0, -5.0);
        break;
    default:
        QChartView::keyPressEvent(event);
        break;
    }
}

void View::keepCallout()
{
    m_tooltip = new Callout(m_chart);
    m_callouts.append(m_tooltip); 
}

void View::tooltip(QPointF point, bool state)
{
    if (m_tooltip == 0)
        m_tooltip = new Callout(m_chart);


    if (state) {
        QString date = QDateTime::fromMSecsSinceEpoch(point.x()).toString(Qt::DateFormat::ISODate);
        date.replace("T"," ");
        m_tooltip->setText(QString("RBS/Cell: %1\nDatetime: %2 \nValue: %3 ").arg(qobject_cast<QLineSeries*>(sender())->name())
                               .arg(date).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}

void View::markerClickHandle()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());


    marker->series()->setVisible(!marker->series()->isVisible());

    marker->setVisible(true);

    qreal alpha = 1.0;

    if (!marker->series()->isVisible())
    alpha = 0.5;

    QColor color;
    QBrush brush = marker->labelBrush();
    color = brush.color();
    color.setAlphaF(alpha);
    brush.setColor(color);
    marker->setLabelBrush(brush);

    brush = marker->brush();
    color = brush.color();
    color.setAlphaF(alpha);
    brush.setColor(color);
    marker->setBrush(brush);

    QPen pen = marker->pen();
    color = pen.color();
    color.setAlphaF(alpha);
    pen.setColor(color);
    marker->setPen(pen);

}

void View::sort(QMap<QString, QList<QStringList> > &data)
{
    // not sure about this
    for(auto count = data.begin(); count != data.end(); ++count ){
          for(int i = 0; i < count.value().size(); i++){
              QDateTime dateTime = QDateTime::fromString(count.value()[i][2],Qt::DateFormat::ISODate);
              if(!dateTime.isValid()) {
                  QString time = count.value()[i][2].split('T')[1];
                  time = count.value()[i][2].split('T')[0] + "T0"+time;
                  count.value()[i][2] = time;
              }
          }
        for(int i = 0; i < count.value().size(); i++){

            for(int j=0;j< count.value().size()-1;j++){
                if(count.value()[j][2] < count.value()[j+1][2]) std::swap(count.value()[j],count.value()[j+1]);
            }
        }
    }
}
