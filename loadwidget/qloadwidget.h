#ifndef QLOADWIDGET_H
#define QLOADWIDGET_H

#include <QObject>
#include <QWidget>
#include <QTextBrowser>

namespace Ui {
class QLoadWidget;
}

class QLoadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QLoadWidget(QWidget *parent = nullptr);
    ~QLoadWidget();

    void setData(const QStringList,const QStringList);

private slots:

    bool subString(QString text, QString findStr) const;
    void updateModel();
    void recalculateModelBars(const int &index);

private:
    Ui::QLoadWidget *ui;

    QStringList cellList, cellPrints;

};

#endif // QLOADWIDGET_H
