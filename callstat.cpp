#include "callstat.h"
#include "ui_callstat.h"

CallStat::CallStat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CallStat)
{
    ui->setupUi(this);
    onStart();
    setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
}

CallStat::~CallStat()
{
    dataBase.close();
    delete ui;
}

void CallStat::onStart()
{
    settings  = new Settings();
    ui->over->setDateTime(QDateTime::currentDateTime());
    ui->begin->setDateTime(QDateTime::currentDateTime());

    this->setWindowTitle("Информация о звонках по определенному сектору");

    this->setWindowIcon(QIcon(":/images/cell.png"));
    ui->pushButton->setIcon(QIcon(":/images/show.png"));
    ui->cell->setPlaceholderText("Укажите здесь CellID( Например: LUG001А , а написать нужно 11)");
}

void CallStat::on_pushButton_clicked()
{
    QSqlDatabase::contains("second") ? dataBase = QSqlDatabase::database("second"):
            dataBase = QSqlDatabase::addDatabase(driver,"second");
    if(driver == "QMYSQL")
    {
        dataBase.setPort(settings->getSettigns("mainForm/mySqlPort").toInt());
        dataBase.setHostName(settings->getSettigns("mainForm/mySqlHost").toString());
        dataBase.setDatabaseName(settings->getSettigns("mainForm/mySqlDatabaseName").toString());
        dataBase.setUserName(settings->getSettigns("mainForm/mySqlUserName").toString());
        dataBase.setPassword(settings->getSettigns("mainForm/mySqlPassword").toString());
    }
    else if (driver == "QPSQL")
    {
        dataBase.setPort(settings->getSettigns("mainForm/postgresPort").toInt());
        dataBase.setHostName(settings->getSettigns("mainForm/postgresHost").toString());
        dataBase.setDatabaseName(settings->getSettigns("mainForm/postgresDatabaseName").toString());
        dataBase.setUserName(settings->getSettigns("mainForm/postgresUserName").toString());
        dataBase.setPassword(settings->getSettigns("mainForm/postgresPassword").toString());
    }
    dataBase.open();
    if(!dataBase.isOpen())
    {
        QMessageBox::information(this,"Database not open",dataBase.lastError().text());
        return;
    }


    if(ui->cell->text().isEmpty())
    {
        QMessageBox::information(this,"Инфо","Введите название сектора");
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(getQuery(),dataBase);
    qDebug() << getQuery();
    ui->tableView->setModel(model);

    while(model->canFetchMore()) model->fetchMore();
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    ui->callCounter->setText("CallCounter:"+QString::number(model->rowCount()));

    int Counter = 0;

    for(int i=0;i<model->rowCount();i++)
    {
        if(model->data(model->index(i,9))==model->data(model->index(i,11))) Counter++;
    }
    ui->limitsCell->setText("OnLimitsOneCell:"+QString::number(Counter));

    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

QString CallStat::getQuery()
{
    QString query = "select tag,callingPartyNumber,calledPartyNumber,dateForStartOfCharge,timeForStartOfCharge,chargeableDuration,"
                    "callingSubscriberIMSI,callingSubscriberIMEI,firstCallingLAC,firstCallingCellID,lastCallingLAC,lastCallingCellID from cdr where firstCallingCellID='"+
    ui->cell->text()+"' and dateForStartOfCharge ";
    if(ui->begin->date()==ui->over->date())
    {
        query+=" = '"+QString::number(ui->begin->date().year())+'-';
        if(ui->begin->date().month()<10) query+='0';
        query+=QString::number(ui->begin->date().month())+'-';
        if(ui->begin->date().day()<10) query+='0';
        query+=QString::number(ui->begin->date().day())+"' ";

    }
    else if(ui->begin->date()!=ui->over->date())
    {
        query +=  "  between '"+QString::number(ui->begin->date().year())+'-';
        if(ui->begin->date().month()<10) query+='0';
        query+=QString::number(ui->begin->date().month()) + '-';
        if(ui->begin->date().day()<10) query+='0';
        query+=QString::number(ui->begin->date().day()) + "' and '"+QString::number(ui->over->date().year())+'-';
        if(ui->over->date().month()<10)query+='0';
        query+=QString::number(ui->over->date().month())+'-';
        if(ui->over->date().day()<10) query+='0';
        query+=QString::number(ui->over->date().day())+"' ";
    }
    if(ui->begin->time()!=ui->over->time())
    {
        query += " and timeForStartOfCharge between '";
        if(ui->begin->time().hour()<10)query+='0';
        query += QString::number(ui->begin->time().hour())+':';
        if(ui->begin->time().minute()<10)query+='0';
        query +=QString::number(ui->begin->time().minute())+":00' and '";
        if(ui->over->time().hour()<10)query+='0';
        query += QString::number(ui->over->time().hour())+':';
        if(ui->over->time().minute()<10) query+='0';
        query+=QString::number(ui->over->time().minute())+":00'";
    }
    qDebug () << query;
    return query;
}

void CallStat::setDriver(QString text)
{
    driver = text;
}
