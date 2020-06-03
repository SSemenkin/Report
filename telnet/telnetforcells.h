#ifndef TELNETFORCELLS_H
#define TELNETFORCELLS_H

#include <QtCore/QObject>
#include <QTcpSocket>
#include <QString>
#include <QFile>
#include "settings.h"


class TelnetForCells : public QObject
{
    Q_OBJECT
public:
    explicit TelnetForCells(QObject *parent = nullptr);
    ~TelnetForCells();

    void getLoadPerCell(const QString rsite);

signals:
    void dataReady(QStringList);
    void executed();

private slots:
    void readDataFromController();
    QString subString(const QString text, const QString buffer);
private:
    uint currentCell = 0;
    QStringList forSignal;
    QString cellText;
    QString rbsName;
    QStringList cellNames;
    uint tg;
    Settings *settings;
    QTcpSocket *telnet;
    QString ipBSC, userNameBSC, passwordBSC;
};

#endif // TELNETFORCELLS_H
