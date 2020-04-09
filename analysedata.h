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

#include "customslice.h"
#include "settings.h"


QT_CHARTS_USE_NAMESPACE


class AnalyseData:public QWidget
{
    Q_OBJECT

public:

    AnalyseData(QSqlQueryModel*model = nullptr,QString abonent = "",QString dates = "");

    AnalyseData();

    ~AnalyseData();


private slots:

    void GenerateChart(QVector<QVector<int>> loadCell,QVector<QVector<int>> icmBand,QVector<QString> dateList,QVector<QString>);

    bool openLocalDataBase();

    void updateWindowWidgets();

    QVector<QVector<int>> prepareData(int columnName,QVector<QString> findCells);

    QVector<QString> Cells();

    QVector<QString> getDateList();

    void xCopy(bool);

    QTextBrowser* getCompleteListCell();

    void swap(QPair<QString,int>  &,QPair<QString,int> &);

    QVector<QColor> getPalitra();

    void setCommands(int);

    void showSettingsWidget();

private:

    QSqlQueryModel *hideModel;
    QString currAbon,interval;

    // already user params
    QVector<QPair<QString,int>> chastoti;

    QVBoxLayout *globalLay;

    QTextEdit *lb;

    QCheckBox *ch2G,*ch3G;

    QSqlDatabase dataBase;



};

#endif // ANALYSEDATA_H
