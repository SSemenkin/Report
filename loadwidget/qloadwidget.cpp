#include "qloadwidget.h"
#include "ui_qloadwidget.h"

QLoadWidget::QLoadWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QLoadWidget)
{
    ui->setupUi(this);
    resize(1400,800);
    ui->progressBar->setAlignment(Qt::AlignHCenter);
    connect(ui->tabWidget,&QTabWidget::currentChanged,this,&QLoadWidget::recalculateModelBars);
}

QLoadWidget::~QLoadWidget()
{
    delete ui;
}

void QLoadWidget::setData(const QStringList a, const QStringList b)
{
    cellList   = a;
    cellPrints = b;
    this->setWindowTitle("Состояние секторов "+cellList[0].left(6));
    updateModel();
}

bool QLoadWidget::subString(QString text, QString findStr) const
{
    text = text.toLower();
    findStr = findStr.toLower();
    for(int i=0;i<text.length()-findStr.length();i++){
        QString tmp = text.right(text.length()-i).left(findStr.length());
        if(tmp == findStr){
            return true;
        }
    }
    return false;
}

void QLoadWidget::updateModel()
{
    for(int i = 0;i<cellPrints.size();i++){
        QTextBrowser *browser = new QTextBrowser(this);
        browser->setText(cellPrints[i]);
        ui->tabWidget->addTab(browser,cellList[i]);
    }

}

void QLoadWidget::recalculateModelBars(const int &index)
{
    uint gprsCount,speechCounter,allTCH;
    gprsCount = speechCounter = allTCH = 0;

    //parse data
    QStringList list = cellPrints[index].split("\n");

    for(int i=0;i<list.size();i++){
        if(subString(list[i],"lu")) {
            for(int j=0;j<list[i].size();j++)
            {
                if(list[i][j]=='-'){
                    QString all;
                    all += list[i][j+1];
                    all += list[i][j+2];
                    all += list[i][j+3];
                    allTCH = all.toUInt();
                }
            }
        }
        if(subString(list[i]," gprs"))  gprsCount++;
        if(subString(list[i],"speech")) speechCounter++;
    }

    ui->progressBar->setValue( double(gprsCount + speechCounter) / double(allTCH) * 100.0 );
    ui->label->setText("Всего каналов(HR):"+QString::number(allTCH)+"\nЗанято под разговоры:"
                       +QString::number(speechCounter)+"\nЗанято под GPRS:"+QString::number(gprsCount)+"\n");

    if(ui->progressBar->value()<=10)  ui->progressBar->setStyleSheet("QProgressBar::chunk {"
                                      "background-color:#D3D3D3;"
                                      "border-radius:5px;"
                                      "}"
                                      "QProgressBar{"
                                      "border-radius:5px;"
                                      "border: 1px solid grey;"
                                      "}");
    else if (ui->progressBar->value()<=50 && ui->progressBar->value()>10) ui->progressBar->setStyleSheet("QProgressBar::chunk {"
                                      "background-color:#ADFF2F;"
                                      "border-radius:5px;"
                                      "}"
                                      "QProgressBar{"
                                      "border-radius:5px;"
                                      "border: 1px solid grey;"
                                      "}");
    else if (ui->progressBar->value()>50 && ui->progressBar->value()<=80) ui->progressBar->setStyleSheet("QProgressBar::chunk {"
                                                       "background-color:#FFD700;"
                                                       "border-radius:5px;"
                                                       "}"
                                                       "QProgressBar{"
                                                       "border-radius:5px;"
                                                       "border: 1px solid grey;"
                                                       "}");
    else ui->progressBar->setStyleSheet("QProgressBar::chunk {"
                            "background-color:red;"
                            "border-radius:5px;"
                            "}"
                            "QProgressBar{"
                            "border-radius:5px;"
                            "border: 1px solid grey;"
                            "}");
}
