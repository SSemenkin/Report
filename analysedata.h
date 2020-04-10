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

#include "customslice.h"
#include "settings.h"


QT_CHARTS_USE_NAMESPACE

struct cell2G
{
    QString cellName;
    QVector<float> charges;
    QVector<QDateTime> dates;
    QVector<int> inter;
};


class AnalyseData:public QWidget
{
    Q_OBJECT

public:

    AnalyseData(QSqlQueryModel*model = nullptr,QString abonent = "",QString dates = "");

    ~AnalyseData();


private slots:

    void GenerateChart(QVector<cell2G>);

    bool openLocalDataBase();

    void updateWindowWidgets();

    QVector<QString> Cells();

    void xCopy(bool);

    void swap(QPair<QString,int>  &,QPair<QString,int> &);

    void setCommands(int);

    void showSettingsWidget();

    QVector<cell2G> getChargeOf2GCellsFromMySQL(QVector<QString>);

    void ShowLoad(QString cell);


private:

    QSqlQueryModel *hideModel;
    QString currAbon,interval;

    // already user params
    QVector<QPair<QString,int>> chastoti;

    QVBoxLayout *globalLay;
    QHBoxLayout *chartsLayout;
    QTextEdit *lb;

    QCheckBox *ch2G,*ch3G;

    QSqlDatabase mysqldataBase;

    Settings *settings;

    QBrush m_originalBrush;

    QVector<QLineSeries*> loadSeriesV;


};

#endif // ANALYSEDATA_H
