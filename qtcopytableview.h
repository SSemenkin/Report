#ifndef QTCOPYTABLEVIEW_H
#define QTCOPYTABLEVIEW_H

#include <QtCore/QObject>
#include <QTableView>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>

class QtCopyTableView : public QTableView
{
    Q_OBJECT
public:
    QtCopyTableView();
    QtCopyTableView(QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // QTCOPYTABLEVIEW_H
