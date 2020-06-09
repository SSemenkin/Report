#ifndef TELNETSEPARATECELL_H
#define TELNETSEPARATECELL_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QTimer>
#include "settings.h"

class TelnetSeparateCell : public QObject
{
    Q_OBJECT
public:
    explicit TelnetSeparateCell(QObject *parent = nullptr);
    ~TelnetSeparateCell();
    enum Operation{
        Default = 0,
        PrintSeparateCell = 1
    };
public slots:
    void getData(Operation choice,const QString cellName);

signals:
    void missSettings();
    void deleteMe();
    void errorCatched(const QString errorText);
    void success(const QString cellName);
    void timeout(const QString cellName);

private slots:
    void readData();
    void proccesingSeparateCell(QString text);
    bool subString(QString buff, QString findStr) const ;
    void moveToFile(const QString content, const QString filePath) const;

private:
    Operation state = Operation::Default;
    QTcpSocket *telnet;
    QString allRecievedData;
    Settings *settings;
    QString cell;
    QString ip, userName, password;
    QString _tg;
    QString responce;
    bool flag = false;

    QTimer timer;
};

#endif // TELNETSEPARATECELL_H
