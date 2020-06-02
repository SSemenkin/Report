#ifndef TELNETREGISTER_H
#define TELNETREGISTER_H

#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>
#include <QTimer>

class TelnetRegister : public QObject
{
    Q_OBJECT
public:
    explicit TelnetRegister(QObject *parent = nullptr);

    void reRegisterAbonent(const QString abonentNumber );

    void printProfileAbonent(const QString abonentNumber);

signals:

    void successed();
    void profileReady(const QString profileText);
    void errorCatched(const QString errorText);
    void executed();

private:

    QString __abonent;
    QString __imsi;
    QString __pdpcp;
    bool firstFlag = false;
    bool is_profile_print = false;

    void processAnswersFromHLR();

    QString subString(const QString text, const QString buffer);

    const QString ipHLR = "10.20.201.20", loginName = "Smena", password = "Ericsson1@";

    QTcpSocket *socket;
    QTimer *timer;

};

#endif // TELNETREGISTER_H
