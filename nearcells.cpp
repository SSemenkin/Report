#include "nearcells.h"
#include "ui_nearcells.h"

NearCells::NearCells(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NearCells)
{
    ui->setupUi(this);
    connect(ui->listWidget,&QListWidget::doubleClicked,[=](const QModelIndex &index){
        TelnetForCells *telnet = new TelnetForCells(this);
        telnet->getLoadPerCell(index.data().toString());
        connect(telnet,&TelnetForCells::dataReady,[=](){
               if(ui->tabWidget->count()){
                   for(int i = ui->tabWidget->count()-1;i>=0;i--) ui->tabWidget->removeTab(i);
               }
               //ui->tabWidget->addTab(makeWidget(data),index.data().toString());
        });
        connect(telnet,&TelnetForCells::executed,telnet,&TelnetForCells::deleteLater);
    });
}

NearCells::~NearCells()
{
    delete ui;
}

void NearCells::setCells(QStringList list)
{
    for(int i = 0; i < list.size(); i++){
        ui->listWidget->insertItem(i,list[i]);
    }
}

QWidget *NearCells::makeWidget(const QString print)
{
   QWidget *result = new QWidget(this);
   QVBoxLayout *layout = new QVBoxLayout(this);
   QTextBrowser *textBrowser = new QTextBrowser(this);
   textBrowser->setFont(QFont("Arial",22));
   textBrowser->setText(print);
   layout->addWidget(textBrowser);
   result->setLayout(layout);
   return result;
}
