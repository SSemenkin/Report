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
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>
#include <QPalette>
#include <QLinearGradient>
#include <QScrollArea>
#include <QDateTime>
#include <QTabWidget>
#include <QThread>
#include <QTimer>
#include <QProgressBar>

#include "customslice.h"
#include "settings.h"
#include "chartbyseparatethread.h"

QT_CHARTS_USE_NAMESPACE



class AnalyseData:public QWidget
{
    Q_OBJECT



public:

    AnalyseData(QSqlQueryModel*model = nullptr,QString abonent = "",QString dates = "",int actionValue = 0);

    ~AnalyseData();


private slots:

    void GenerateChart();

    void updateWindowWidgets();

    QVector<QString> Cells();

    void xCopy(bool);

    void swap(QPair<QString,int>  &,QPair<QString,int> &);

    void setCommands(int);

    void showSettingsWidget();

    void ShowLoad(QString cell);

    QChartView *buildPieChart();

    QTabWidget *buildLineCharts(QVector<cell2G> cells2G);

    void storeResult();

    void repaintModel();

    void changeText();

private:


    QSqlQueryModel *hideModel;
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
