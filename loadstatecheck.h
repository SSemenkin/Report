#ifndef LOADSTATECHECK_H
#define LOADSTATECHECK_H

#include <QObject>
#include <QDialog>
#include <QWidget>
#include <QFile>
#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QGroupBox>
#include <QProgressBar>
#include <QIcon>
#include <QTabWidget>
#include <QStyle>
#include <QPalette>
#include <QColor>
#include <QTime>

class LoadStateCheck:public QWidget
{
public:
    LoadStateCheck(QString);

    ~LoadStateCheck();

    bool subString(QString buff, QString findStr);

    void makeModel(QStringList allTextCell,QString msp,int,int,int);

    QStringList makeTextBeautiful(QStringList txt);
};

#endif // LOADSTATECHECK_H
