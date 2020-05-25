#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QObject>
#include <QLineEdit>
#include <QKeyEvent>
#include <QComboBox>

class MyLineEdit : public QLineEdit
{

    Q_OBJECT

public:
    MyLineEdit();

protected:
    void keyPressEvent(QKeyEvent *e) ;

signals:
    void command(QString cell);
    void letsGetEDR(QString);
};

#endif // MYLINEEDIT_H
