#ifndef NEARCELLS_H
#define NEARCELLS_H

#include <QWidget>
#include <QObject>
#include <QMessageBox>
#include <QTextBrowser>
#include "telnet/telnetforcells.h"

namespace Ui {
class NearCells;
}

class NearCells : public QWidget
{
    Q_OBJECT

public:
    explicit NearCells(QWidget *parent = nullptr);
    ~NearCells();

    void setCells(QStringList list);

    QWidget* makeWidget(const QString print);

private:
    Ui::NearCells *ui;

};

#endif // NEARCELLS_H
