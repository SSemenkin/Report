#ifndef TELNETREGISTER_H
#define TELNETREGISTER_H

#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>
#include <QTimer>
#include <QFile>
#include <QDateTime>

class TelnetRegister : public QObject
{
    Q_OBJECT
public:
    explicit TelnetRegister(QObject *parent = nullptr);

    void reRegisterAbonent(const QString abonentNumber );

    void printProfileAbonent(const QString abonentNumber);

    void changePDPCP(const QString abonentNumber);

signals:

    void successed();
    void profileReady(const QString profileText);
    void errorCatched(const QString errorText);
    void executed();

private:
    enum OperationBehavior{
        PrintProfile, ChangeSUD, Reregister
    };

    QString __abonent;
    QString __imsi;
    QString __pdpcp;
    QString __caw;
    OperationBehavior __behavior;
    uint countCommands = 0;

    QString allOperationText;

    void processAnswersFromHLR();

    void validateNumber(const QString number);

    QString subString(const QString text, const QString buffer);

    void loging(QString erroText = "") const ;

    const QString ipHLR = "10.20.201.20", loginName = "Smena", password = "Ericsson1@";

    QTcpSocket *socket;
    QTimer *timer;


};

#endif // TELNETREGISTER_H
