#ifndef ANALYSEDATA_H
#define ANALYSEDATA_H

#include <QMainWindow>
#include <QWidget>
#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QString>
#include <QVector>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QtCharts/QtCharts>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>
#include <QScrollArea>
#include <QDateTime>
#include <QTabWidget>
#include <QThread>
#include <QTimer>
#include <QProgressBar>

#include "customslice.h"
#include "settings.h"
#include "chartbyseparatethread.h"
#include "charts/donutbreakdownchart.h"
#include "charts/mainslice.h"

QT_CHARTS_USE_NAMESPACE



class AnalyseData:public QWidget
{
    Q_OBJECT



public:

    AnalyseData(QSqlQueryModel*model = nullptr,QString abonent = "",QString dates = "",
                int actionValue = 0,QSqlQueryModel *edrModel = nullptr);

    ~AnalyseData();


private slots:

    void GenerateChart();

    void updateWindowWidgets();

    QVector<QString> Cells();

    void xCopy(bool);

    void swap(QPair<QString,int>  &,QPair<QString,int> &);

    void setCommands(int);

    void ShowLoad(QString cell);

    QChartView *buildPieChart();

    QChartView *buildEDRPieChart(QMap<QString,int>);

    QTabWidget *buildLineCharts(QVector<cell2G> cells2G);

    QTabWidget *buildPieCharts();

    QChartView *buildDonutChart(QMap<QString,QMap<QString,double>> data);

    void storeResult();

    void repaintModel();

    void changeText();

    QMap<QString,QMap<QString,double>> calculateFrequences(QSqlQueryModel* model,int flagCells = 0);

signals:
    void threadFinished(bool);

private:

    bool is_db_open = true;

    QSqlQueryModel *hideModel,*hideEdrModel;
    QString currAbon,interval;

    // already user params
    QVector<QPair<QString,int>> chastoti;

    QVBoxLayout *globalLay;

    QHBoxLayout *chartsLayout;

    QTextEdit *lb;

    QCheckBox *ch2G,*ch3G;

    Settings *settings;

    QVector<QLineSeries*> loadSeriesV,interSeries;

    QVector<CustomSlice*> customSlices;

    int days;

    QThread *threadToSQLDatabase;
    ChartBySeparateThread *sepChartClass;

    QVector<QString> cells;

    QVector<cell2G> returnedData;

    QLabel *label;

    QProgressBar *dataBar;

    QTimer *timer;

    QVBoxLayout *tmpLay;

};

#endif // ANALYSEDATA_H
