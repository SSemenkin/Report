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
    void errorCatched(const QString errorText);
    void dataReady(QStringList cellNames, QStringList prints);
    void executed();

private slots:
    void readDataFromController();
    QString subString(const QString text, const QString buffer);
    QStringList parseCellList(QString answer);
private:
    uint currentCell = 0;
    bool firstFlag = false;
    QStringList prints;
    QString cellText;
    QString rbsName;
    QStringList cellNames;
    uint tg;
    Settings *settings;
    QTcpSocket *telnet;
    QString ipBSC, userNameBSC, passwordBSC;
};

#endif // TELNETFORCELLS_H
