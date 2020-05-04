#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings = new Settings();
    edr_model = new QSqlQueryModel();
    onStart();
    DbConnect();
    AbonentSelected = false;

}

MainWindow::~MainWindow()
{
    QStringList a = db.connectionNames();
    for(int i = 0; i < a.size() ;i++)
    {
        if(a[i]!="QMYSQL_detail_connection")
        {
            db = QSqlDatabase::database(a[i]);
            db.close();
        }
    }
    delete completer;
    delete abonentLine;
    delete IMEI;
    delete customMenu;
    delete actionDay;
    delete actionWeek;
    delete model;
    delete edr_model;
    delete settings;
    delete ui;
}

QString MainWindow::getDatesToChart()
{
    QString requestPart;
    if(ui->oneDay->isChecked())
    {
        requestPart = " and records.date_charge = '"+QString::number(ui->dateEdit_2->date().year())+'-';
        if(ui->dateEdit_2->date().month()<10) requestPart +='0';
        requestPart += QString::number(ui->dateEdit_2->date().month())+'-';
        if(ui->dateEdit_2->date().day()<10) requestPart +='0';
        requestPart += QString::number(ui->dateEdit_2->date().day())+"'";
    }
    else if(ui->period->isChecked())
    {
        requestPart = " and records.date_charge between '"+QString::number(ui->dateEdit_2->date().year())+'-';
        if(ui->dateEdit_2->date().month()<10) requestPart +='0';
        requestPart += QString::number(ui->dateEdit_2->date().month())+'-';
        if(ui->dateEdit_2->date().day()<10) requestPart +='0';
        requestPart += QString::number(ui->dateEdit_2->date().day())+"' and '"+QString::number(ui->dateEdit->date().year())+'-';
        if(ui->dateEdit->date().month()<10) requestPart +='0';
        requestPart += QString::number(ui->dateEdit->date().month())+'-';
        if(ui->dateEdit->date().day()<10) requestPart +='0';
        requestPart += QString::number(ui->dateEdit->date().day())+"'";
    }
    return requestPart;
}

void MainWindow::onStart()
{ 

    ui->driverCombo->addItems({"QPSQL","QMYSQL"});
    completer = new QCompleter(sql.toLower().split("\r\n"),this);
    //completer->setModel(modelFromFile("wordlist.txt"));
    //completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);
    ui->query->setCompleter(completer);

    abonentLine = new MyLineEdit();
    IMEI = new MyLineEdit();

    ui->horizontalLayout_11->insertWidget(1,IMEI);
    ui->horizontalLayout_3->insertWidget(0,abonentLine);

    setWindowIcon(QIcon(":/images/main.ico"));

    ui->oneDay->setIcon(QIcon(":/images/day.png"));
    ui->period->setIcon(QIcon(":/images/period.png"));
    ui->radioButton_2->setIcon(QIcon(":/images/vor.png"));


    on_radioButton_clicked();
    ui->columnOrder->addItem("dateForStartOfCharge");
    ui->columnOrder->addItem("timeForStartOfCharge");
    ui->columnOrder->addItem("chargeableDuration");
    ui->favoriteNumber->setVisible(false);
    ui->duration->setVisible(false);
    ui->line_2->setVisible(false);



    ui->groupBox->setFixedWidth(290);
    ui->groupBox_2->setFixedWidth(650);
    ui->groupBox_6->setFixedWidth(300);
    ui->groupBox->setAlignment(Qt::AlignHCenter);
    ui->groupBox_2->setAlignment(Qt::AlignLeft);
    ui->groupBox_6->setAlignment(Qt::AlignRight);

    ui->groupBox_7->setFixedHeight(150);

    ui->verticalLayout_4->setAlignment(ui->saveToFile,Qt::AlignHCenter);


    abonentLine->setFixedWidth(250);
    ui->pastePB->setFixedWidth(50);

    ui->showData->setShortcut(QKeySequence(tr("Ctrl+L")));
    ui->pastePB->setIcon(QIcon(":/images/xCopy.png"));
    ui->pushButton_2->setIcon(QIcon(":/images/execute.png"));
    ui->pushButton_3->setIcon(QIcon(":/images/clear-symbol.png"));
    ui->showData->setIcon(QIcon(":/images/show.png"));
    ui->pushButton->setIcon(QIcon(":/images/save.png"));
    ui->connect->setIcon(QIcon(":/images/connect.png"));
    ui->analyseDataButton->setIcon(QIcon(":/images/PieChart.png"));
    ui->chartButton->setIcon(QIcon(":/images/chart.png"));
    ui->pushButton_5->setIcon(QIcon(":/images/help.png"));
    ui->asc->setIcon(QIcon(":/images/asc.png"));
    ui->desc->setIcon(QIcon(":/images/desc.png"));
    ui->incoming->setIcon(QIcon(":/images/incoming.png"));
    ui->outgoing->setIcon(QIcon(":/images/outgoing.png"));
    ui->all_calls->setIcon(QIcon(":/images/all_calls.png"));
    ui->cellStat->setIcon(QIcon(":/images/cell.png"));


    ui->verticalLayout_4->setAlignment(ui->saveToFile,Qt::AlignLeft);

    ui->radioButton->setChecked(true);
    ui->outgoing->setToolTip("Указывать номер нужно в формате 72XXXXXXX");
    ui->incoming->setToolTip("Указывать номер нужно в формате 72XXXXXXX");
    ui->all_calls->setToolTip("Указывать номер нужно в формате 72XXXXXXX");
    ui->query->setPlaceholderText("Напишите SQL запрос...");
    ui->pushButton_5->setToolTip("Отобразить даты, присутствующие в БД");
    requestBegin = "select tag,callingPartyNumber,calledPartyNumber,dateForStartOfCharge,timeForStartOfCharge,chargeableDuration,"
                   "callingSubscriberIMSI,callingSubscriberIMEI,firstCallingLAC,firstCallingCellID,lastCallingLAC,lastCallingCellID from mss";

    if(!QDir("results").exists()) QDir().mkdir("results");
    if(!QDir("results\\MN").exists()) QDir().mkdir("results\\MN");
    if(!QDir("results\\RU").exists()) QDir().mkdir("results\\RU");
    if(!QDir("results\\UA").exists()) QDir().mkdir("results\\UA");
    if(!QDir("results\\CITY").exists()) QDir().mkdir("results\\CITY");
    if(!QDir("results\\Abonent").exists()) QDir().mkdir("results\\Abonent");
    if(!QDir("results\\IMEI").exists()) QDir().mkdir("results\\IMEI");



    ui->comboBox->addItem("Россия");
    ui->comboBox->addItem("Украина");
    ui->comboBox->addItem("Международные (кроме России и Украины)");
    ui->comboBox->addItem("На городские (064)");
    model = new QSqlQueryModel(this);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit_2->setDate(QDate::currentDate());
    ui->comboBox->setFixedWidth(250);

    IMEI->setValidator(new QRegExpValidator(QRegExp("[0-9]{15}")));
    abonentLine->setValidator(new QRegExpValidator(QRegExp("[0-9]{15}")));
    ui->port->setValidator(new QIntValidator());

    request_MN = requestBegin + " WHERE calledPartyNumber not like '380%"
                 "' and tag = '1' and calledPartyNumber not like '7%' and calledPartyNumber "
                 "not like '72%' and calledPartyNumber not like '072%' and calledPartyNumber not like '071%' "
                 "and calledPartyNumber not like '71%' and calledPartyNumber  <> 101 and calledPartyNumber  <> 102 "
                 "and calledPartyNumber  <> 103  and calledPartyNumber  <> 104 and calledPartyNumber  <> 177 "
                 "and calledPartyNumber  <> 303 and calledPartyNumber  <> 422 and calledPartyNumber <> 300 and calledPartyNumber not like '064%' "
                 "and calledPartyNumber <> 505  and calledPartyNumber <> 333 and calledPartyNumber <> 109 and calledPartyNumber not like '062%'";

    abonentLine->setFocus();
    connect(ui->actionExit,&QAction::triggered,this,&QApplication::quit);
    connect(abonentLine,&MyLineEdit::command,this,&MainWindow::on_showData_clicked);
    connect(IMEI,&MyLineEdit::command,this,&MainWindow::on_showData_clicked);
    connect(ui->chartButton,&QPushButton::clicked,this,&MainWindow::execAnalyseWithLoadChart);



    ui->groupBox_2->setFixedWidth(850);
    ui->groupBox->setAlignment(Qt::AlignLeft);
    ui->groupBox_6->setAlignment(Qt::AlignLeft);
    ui->dateEdit_2->setVisible(false);
    ui->dateEdit->setVisible(false);
    //chart Button and context menu
    ui->chartButton->setContextMenuPolicy(Qt::CustomContextMenu);
    actionDay = new QAction(QIcon(":/images/day.png"),"Данные загрузки за 3 дня");
    actionWeek = new QAction(QIcon(":/images/week.png"),"Данные загрзуки за неделю");
    customMenu = new QMenu(this);
    customMenu->addAction(actionDay);
    customMenu->addAction(actionWeek);
    connect(actionDay,&QAction::triggered,this,&MainWindow::actionChartButtonTriggered);
    connect(actionWeek,&QAction::triggered,this,&MainWindow::actionChartButtonTriggered);
    // over
    this->showMaximized();
}

void MainWindow::DbConnect()
{

        if(!ui->host->text().isEmpty())
        {
            db = QSqlDatabase::addDatabase(ui->driverCombo->currentText(),ui->driverCombo->currentText()+"first_connection");
            db.setHostName(ui->host->text());
            db.setPort(ui->port->text().toInt());
            db.setDatabaseName(ui->DBname->text());
            db.setUserName(ui->login->text());
            db.setPassword(ui->password->text());
            db.open();

            if(db.isOpen())
            {

                if(ui->driverCombo->currentText() == "QPSQL")
                {
                    openDatabaseEDR();
                }
                else
                {
                    dataBaseEDR.close();
                }
                ui->statusBar->showMessage("Статус соединения с БД: Соединение установлено",5000);
                ui->connectionLabel->setText("Текущее подключение: \n"+db.connectionName());
            }
            else
            {
                db.close();
                ui->connectionLabel->setText("Текущее подключение: \nNone");
                ui->statusBar->showMessage("Статус соединения с БД: Соединение не установлено",5000);
            }
        }
}

void MainWindow::on_showData_clicked()
{
    if(abonentLine->text().left(3)=="380")
        abonentLine->setText(abonentLine->text().right(abonentLine->text().length()-3));
    if(!db.isOpen())
    {
        QMessageBox::information(this,"Error",db.lastError().text()+'\n'+model->query().lastError().text()+'\n'+db.driver()->lastError().text());
        return;
    }
    ui->tabWidget->setCurrentWidget(ui->tab);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    abonentAnalyse(-1);
    if(dataBaseEDR.isOpen() && ui->radioButton->isChecked())
    {
        QString request = "select datecharge,eventidentity,result,radiotype,typerau,intrarautype"
                          ",gsmcause,discreason,rai,cellidorsai,sac,msisdn,imsi,ptmsi,hlrnumber,sizeapn,apn,oldrai,negotiatedqos  from edr where msisdn = '380"+abonentLine->text()+"'";
        edr_model->setQuery(request,dataBaseEDR);

        QSortFilterProxyModel *sortedModel = new QSortFilterProxyModel(this);
        sortedModel->setDynamicSortFilter(true);
        sortedModel->setSourceModel(edr_model);

        ui->tableViewEDR->setModel(sortedModel);
        ui->tableViewEDR->resizeRowsToContents();
        ui->tableViewEDR->resizeColumnsToContents();
        ui->tableViewEDR->resizeRowsToContents();
        ui->tableViewEDR->resizeColumnsToContents();

    }

    ui->statusBar->showMessage("");
    if(ui->radioButton_5->isChecked() && ui->comboBox->currentIndex()==0)
    {
        //Россия
        QString request = requestBegin +" where tag='1' and calledPartyNumber like '7%' "+DateB()+";";
        model->setQuery(request,db);
        SetModel(request);

    }
    if(ui->radioButton_5->isChecked() && ui->comboBox->currentIndex()==1)
    {
       //Украина
        QString request = requestBegin + " where tag='1' and calledPartyNumber like '380%' and calledPartyNumber not like '38072%' and calledPartyNumber not "
                          "like '72%' and calledPartyNumber not like '072%' and calledPartyNumber not like '38071%' and calledPartyNumber not like '071%' and calledPartyNumber not like '71%' and "
                                         "calledPartyNumber not like '380642%' and calledPartyNumber not like '3806431%' and calledPartyNumber not like '3806442%' and calledPartyNumber not like '3806443%' "
                                         "and calledPartyNumber not like '3806446%' and calledPartyNumber not like '3806435%' "
                                         "and calledPartyNumber not like '3806432%' and calledPartyNumber not like '3806436%' and calledPartyNumber not like '3806455%' and calledPartyNumber not like '3806441%' "
                                         "and calledPartyNumber not like '3806433%' and calledPartyNumber not like '3806434%' and calledPartyNumber not like '3806444%' and calledPartyNumber not like '3806473%'  " +DateB()+";";
        model->setQuery(request,db);
        SetModel(request);


    }
    if(ui->radioButton_5->isChecked() && ui->comboBox->currentIndex()==2)
    {
        //Международка

        QString request = "";
        request = request_MN +" "+DateB()+";";

        model->setQuery(request,db);

        int calledIndex = 0;
        for(int i=0;i<model->columnCount();i++)
            if(model->headerData(i,Qt::Horizontal).toString()=="calledPartyNumber")
            {
                calledIndex = i;
                break;
            }

        QVector<QString> Data,Data2;
        QString shapka;
        for(int i=0;i<model->columnCount();i++)
            i<model->columnCount()-1? shapka+=model->headerData(i,Qt::Horizontal).toString()+';':shapka+=model->headerData(i,Qt::Horizontal).toString();

        Data.push_back(shapka);
        Data2.push_back(shapka);
        QString tmp;
        for(int i=0;i<model->rowCount();i++)
        {

             if(model->data(model->index(i,calledIndex)).toString().length()>5)
             {

               for(int j=0;j<model->columnCount();j++)
               {
                    tmp+=model->data(model->index(i,j)).toString();
                    if(j<model->columnCount()-1) tmp+=';';
               }
               Data.push_back(tmp);
               tmp.clear();
             }
             else
             {
                 for(int j=0;j<model->columnCount();j++)
                 {
                      tmp+=model->data(model->index(i,j)).toString();
                      if(j<model->columnCount()-1) tmp+=';';
                 }
                 Data2.push_back(tmp);
                 tmp.clear();
             }

        }

       QFile file;
       QString filePath = " MN";
       if(ui->radioButton_2->isChecked())  filePath +="-All time";
       else if(ui->oneDay->isChecked())
       {
           QString date;
           ui->dateEdit_2->date().day()<10 ? date = '0'+QString::number(ui->dateEdit_2->date().day()) : date = QString::number(ui->dateEdit_2->date().day());
           ui->dateEdit_2->date().month()<10 ? date += '0'+QString::number(ui->dateEdit_2->date().month()):date += QString::number(ui->dateEdit_2->date().month());
           date+=QString::number(ui->dateEdit_2->date().year());

           filePath = "results\\MN\\"+date + filePath;
       }
       else
       {
           QString date;
           ui->dateEdit_2->date().day()<10 ? date = '0'+QString::number(ui->dateEdit_2->date().day()) : date = QString::number(ui->dateEdit_2->date().day());
           ui->dateEdit_2->date().month()<10 ? date += '0'+QString::number(ui->dateEdit_2->date().month()):date += QString::number(ui->dateEdit_2->date().month());
           date+=QString::number(ui->dateEdit_2->date().year()) + '-';

           ui->dateEdit->date().day()<10 ? date += '0'+QString::number(ui->dateEdit->date().day()) : date += QString::number(ui->dateEdit->date().day());
           ui->dateEdit->date().month()<10 ? date += '0'+QString::number(ui->dateEdit->date().month()):date += QString::number(ui->dateEdit->date().month());
           date+=QString::number(ui->dateEdit->date().year());
           filePath = "results\\MN\\"+date + filePath;
       }
       filePath+=".csv";
       file.setFileName(filePath);
       file.open(QIODevice::WriteOnly);
       QTextStream out(&file);

       for(int i=0;i<Data.size();i++)
           out<< Data[i]<<"\r\n";
       file.close();

       QString filePathMilitary = "results\\LastResult-Military.csv";
       file.setFileName(filePathMilitary);
       file.open(QIODevice::WriteOnly);
       for(int i=0;i<Data2.size();i++)
           out << Data2[i]<< "\r\n";

       ui->statusBar->showMessage("*Внимание! Здесь отображены данные как о МН-направлениях, так и о вызовах на военные номера(корректные данные сохранены в файлах)");


       QMessageBox::information(this,"Info","Данные о международных вызовах сохранены в файле:\r\n"+filePath+"\r\nДанные о вызовах на военные номера в файле:\r\n"+filePathMilitary);
        SetModel(request);
        QApplication::setOverrideCursor(Qt::ArrowCursor);
       return;

    }
    if(ui->radioButton_4->isChecked())
    {
        //Поиск по IMEI
        if(IMEI->text().isEmpty())
        {
            QMessageBox::critical(this,"Error","Пустой IMEI");
            QApplication::setOverrideCursor(Qt::ArrowCursor);
            return;
        }

        QString request = requestBegin + " where tag='1' and callingSubscriberIMEI like '%"+IMEI->text()+"%' " + DateB()+";";
        model->setQuery(request,db);
        SetModel(request);

    }
    if(ui->radioButton_5->isChecked() && ui->comboBox->currentIndex()==3)
    {
        //Городские

        QString request = requestBegin + " where tag='1' and ((calledPartyNumber like '0642%' or calledPartyNumber like '380642%') or"
                                         "(calledPartyNumber like '06431%' or calledPartyNumber like '3806431%') or "
                                         "(calledPartyNumber like '06442%' or calledPartyNumber like '3806442%') or "
                                         "(calledPartyNumber like '06443%' or calledPartyNumber like '3806443%') or "
                                         "(calledPartyNumber like '06446%' or calledPartyNumber like '3806446%') or "
                                         "(calledPartyNumber like '06435%' or calledPartyNumber like '3806435%') or "
                                         "(calledPartyNumber like '06432%' or calledPartyNumber like '3806432%') or "
                                         "(calledPartyNumber like '06436%' or calledPartyNumber like '3806436%') or "
                                         "(calledPartyNumber like '06455%' or calledPartyNumber like '3806455%') or "
                                         "(calledPartyNumber like '06441%' or calledPartyNumber like '3806441%') or "
                                         "(calledPartyNumber like '06433%' or calledPartyNumber like '3806433%') or "
                                         "(calledPartyNumber like '06434%' or calledPartyNumber like '3806434%') or "
                                         "(calledPartyNumber like '06444%' or calledPartyNumber like '3806444%') or "
                                         "(calledPartyNumber like '06473%' or calledPartyNumber like '3806473%'))  "+DateB()+";";
        model->setQuery(request,db);
        SetModel(request);
    }

    if(ui->radioButton->isChecked() && ui->outgoing->isChecked())
    {

         //Исходящие

        if(abonentLine->text().isEmpty())
        {
            QMessageBox::information(this,"Информация","Введите номер");
            QApplication::setOverrideCursor(Qt::ArrowCursor);
            return;
        }

        QString request;
        abonentLine->text().isEmpty() ? request = requestBegin + " where tag='1' " +DateB()+";"
                : request = requestBegin + " where tag='1' and (callingPartyNumber = '"+abonentLine->text()+"' or callingPartyNumber ='0"+abonentLine->text()+"' or callingPartyNumber ='380"+abonentLine->text()+"' )" + DateB()+";";

        model->setQuery(request,db);
        SetModel(request);

        if(model->query().isSelect() && model->rowCount()>0)
        {
            lastSelectedAbonent = abonentLine->text();
            abonentAnalyse(2);
        }

    }

    if(ui->radioButton->isChecked() && ui->incoming->isChecked())
    {
        //Входящие

        if(abonentLine->text().isEmpty())
        {
            QMessageBox::information(this,"Информация","Введите номер");
            QApplication::setOverrideCursor(Qt::ArrowCursor);
            return;
        }

        QString request;

         abonentLine->text().isEmpty()?  request = requestBegin + " where tag='1' " +DateB()+";"
                 :request = requestBegin + " where tag='1' and (calledPartyNumber = '380"+abonentLine->text()+
                 "' or calledPartyNumber ='"+abonentLine->text()+"' or calledPartyNumber = '0"+abonentLine->text()+"') " + DateB()+";";
        model->setQuery(request,db);

        SetModel(request);
        if(model->query().isSelect() && model->rowCount()>0)
        {
            lastSelectedAbonent = abonentLine->text();
            abonentAnalyse(1);
        }


    }
    if(ui->radioButton->isChecked() && ui->all_calls->isChecked())
    {
        //Все вызовы

        if(abonentLine->text().isEmpty())
        {
            QApplication::setOverrideCursor(Qt::ArrowCursor);
            QMessageBox::information(this,"Информация","Введите номер.");
            return;
        }

        QString request;
         abonentLine->text().isEmpty() ? request = requestBegin + " where tag='1' " +DateB()+";" :
                 request = requestBegin + " where tag = '1' and ((calledPartyNumber = '380"+abonentLine->text()+
                 "' or calledPartyNumber = '0"+abonentLine->text()+"' or calledPartyNumber='"+abonentLine->text()+"') or (callingPartyNumber = '"
                +abonentLine->text()+"' or callingPartyNumber='380"+abonentLine->text()+"' or callingPartyNumber='0"+abonentLine->text()+"')) " + DateB()+";";
        model->setQuery(request,db);

        SetModel(request);
        if(model->query().isSelect() && model->rowCount()>0)
        {
            lastSelectedAbonent = abonentLine->text();
        }

    }
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    if(ui->saveToFile->isChecked() && model->query().isSelect())
    {
        if(ui->radioButton->isChecked())
            SaveToFile(ui->radioButton->text());
        if(ui->radioButton_4->isChecked())
            SaveToFile(ui->radioButton_4->text());
        if(ui->radioButton_5->isChecked())
            SaveToFile(ui->comboBox->currentText());
    }
    if(ui->radioButton->isChecked() && model->rowCount()>0)
    {
        AbonentSelected = true;
        ui->analyseDataButton->setEnabled(true);
        ui->chartButton->setEnabled(true);
    }
    else
    {
        ui->analyseDataButton->setEnabled(false);
        ui->chartButton->setEnabled(false);
        AbonentSelected = false;
    }

    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

QString MainWindow::DateB()
{
    if(ui->period->isChecked())
    {
        QString result = " and dateForStartOfCharge between '"
                +QString::number(ui->dateEdit_2->date().year())+"-";
        if(ui->dateEdit_2->date().month()<10) result+="0";
        result+=QString::number(ui->dateEdit_2->date().month())+"-";
        if(ui->dateEdit_2->date().day()<10) result+="0";
        result+=QString::number(ui->dateEdit_2->date().day())+"' and '"+QString::number(ui->dateEdit->date().year())+"-";
        if(ui->dateEdit->date().month()<10) result+="0";
        result+=QString::number(ui->dateEdit->date().month())+"-";
        if(ui->dateEdit->date().day()<10) result+="0";
        result+=QString::number(ui->dateEdit->date().day())+"'";

        if(ui->chBoxEnableOrder->isChecked())
        {
            result +=" order by "+ui->columnOrder->currentText()+" ";

            ui->asc->isChecked() ? result+=" asc " :
                    result+=" desc ";
        }

        return result;
    }
    else if(ui->oneDay->isChecked())
    {
        QString result = " and dateForStartOfCharge = '";
        result +=QString::number(ui->dateEdit_2->date().year())+"-";
        QString::number(ui->dateEdit_2->date().month())>10?result+=QString::number(ui->dateEdit_2->date().month())+'-' : result+='0'+QString::number(ui->dateEdit_2->date().month())+'-';
        QString::number(ui->dateEdit_2->date().day())>10?result+=QString::number(ui->dateEdit_2->date().day()) : result+='0'+QString::number(ui->dateEdit_2->date().day());
        result+="'";

        if(ui->chBoxEnableOrder->isChecked())
        {
            result +=" order by "+ui->columnOrder->currentText()+" ";

            ui->asc->isChecked() ? result+=" asc " :
                    result+=" desc ";
        }
        return result;
    }

    QString result;
    if(ui->chBoxEnableOrder->isChecked())
    {
        result +=" order by "+ui->columnOrder->currentText()+",timeforstartofcharge ";

        ui->asc->isChecked() ? result+=" asc " :
                result+=" desc ";
    }
    return result;
}

void MainWindow::on_pushButton_clicked()
{
    if(ui->tableViewQuery->model()!=nullptr)
    {
        if(ui->tableViewQuery->model()->rowCount()>0)
        {
            QString path  = QFileDialog::getSaveFileName(this,"Save to file",QDir::homePath(), tr("Microsoft Excell documents (*.xlsx);;Coma-separated values (*.csv)"));
            QVector<QString> Data;
            QString tmp;
            for(int i=0;i<query_model->columnCount();i++)
            {
                tmp += query_model->headerData(i,Qt::Horizontal).toString();
                if(i<query_model->columnCount()-1) tmp+=';';
            }
            Data.push_back(tmp);
            tmp.clear();
            for(int i=0;i<query_model->rowCount();i++)
            {
                for(int j=0;j<query_model->columnCount();j++)
                    j<query_model->columnCount()-1? tmp+=query_model->data(query_model->index(i,j)).toString()+";":tmp+=query_model->data(query_model->index(i,j)).toString();
                Data.push_back(tmp);
                tmp.clear();
            }
            if(path.right(3)=="csv")
            {

                QFile file;
                file.setFileName(path);
                file.open(QIODevice::WriteOnly);
                QTextStream out(&file);
                for (int i =0;i<Data.size();i++)
                {
                    out << Data[i] +"\r\n";
                }
            }
            else
            {
                QXlsx::Document xlsx;

                for(int i=0;i<query_model->columnCount();i++)
                    xlsx.write(1,i+1,query_model->headerData(i,Qt::Horizontal).toString());

                for(int i=0;i<query_model->rowCount();i++)
                {
                    for(int j=0;j<query_model->columnCount();j++)
                        xlsx.write(i+2,j+1,query_model->index(i,j).data().toString());
                }
                xlsx.saveAs(path);
            }
        }
    }
    else QMessageBox::information(this,"Ошибка","Модель запросов не определена");
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->query->clear();
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->tab_2);
    ui->analyseDataButton->setEnabled(0);
    ui->chartButton->setEnabled(false);
    AbonentSelected = false;
    if(!db.isOpen())
    {
        QMessageBox::critical(this,"Ошибка",db.lastError().text());
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        return;
    }
     QApplication::setOverrideCursor(Qt::WaitCursor);
    if(ui->query->toPlainText().isEmpty())
    {
       ui->statusBar->showMessage("Empty Query (пустой запрос).");
       return;
    }
      query_model = new QSqlQueryModel(ui->tableViewQuery);
      query_model->setQuery(ui->query->toPlainText(),db);


      if(query_model->lastError().type() != QSqlError::NoError)
        ui->statusBar->showMessage(query_model->lastError().text());
      else if(query_model->query().isSelect()){
        ui->statusBar->showMessage("Query Ok.");

        QSortFilterProxyModel *sortedModel = new QSortFilterProxyModel(this);
        sortedModel->setDynamicSortFilter(true);
        sortedModel->setSourceModel(query_model);

         ui->tableViewQuery->setModel(sortedModel);
        }
      else {
          ui->statusBar->showMessage(tr("Query OK, number of affected rows: %1").arg(
                           query_model->query().numRowsAffected()));
           ui->tableViewQuery->setModel(query_model);
          }
     QApplication::setOverrideCursor(Qt::ArrowCursor);

    ui->label_7->setText("Строк:"+QString::number(query_model->rowCount()));


    ui->tableViewQuery->resizeColumnsToContents();
    ui->tableViewQuery->resizeRowsToContents();
}

void MainWindow::GetTime()
{
    //in seconds
    int seconds = 0;

    for(int j =0;j<model->rowCount();j++)
    {
        seconds += model->data(model->index(j,5)).toInt();
    }
    ui->label_8->setText("В секундах: " +QString::number(seconds));

    int minuts = 0;

    for(int j=0;j<model->rowCount();j++)
    {
        int tmp_sec = model->data(model->index(j,5)).toInt();
        if(tmp_sec>2)
        {
           minuts +=tmp_sec/60;
           if(tmp_sec%60>=0) minuts++;
        }
    }

    ui->label_9->setText(+"В минутах: " + QString::number(minuts));
}

void MainWindow::SetModel(QString request)
{
    QSortFilterProxyModel *sortedModel = new QSortFilterProxyModel(this);
    sortedModel->setDynamicSortFilter(true);
    sortedModel->setSourceModel(model);
    ui->tableView->setModel(sortedModel);
    ui->label_7->setText("Строк(Вызовов): "+QString::number(model->rowCount()));
    ui->tableView->resizeColumnsToContents();
    ui->tableViewEDR->resizeRowsToContents();
    ui->tableViewEDR->resizeColumnsToContents();
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    ui->query->setText(request);
    GetTime();
}

void MainWindow::on_connect_clicked()
{
    QSqlDatabase::contains(ui->driverCombo->currentText()+"first_connection") ? db = QSqlDatabase::database(ui->driverCombo->currentText()+"first_connection") :
            db = QSqlDatabase::addDatabase(ui->driverCombo->currentText(),ui->driverCombo->currentText()+"first_connection");

    db.setHostName(ui->host->text());
    db.setPort(ui->port->text().toInt());
    db.setDatabaseName(ui->DBname->text());
    db.setUserName(ui->login->text());
    db.setPassword(ui->password->text());
    db.open();
    if(db.isOpen())
    {
        ui->statusBar->showMessage("Статус соединения с БД: Соединение установлено",5000);
        ui->connectionLabel->setText("Текущее подключение: \n"+db.connectionName());
    }
    else
    {
        ui->statusBar->showMessage("Статус соединения с БД: Соединение не установлено",5000);
        ui->connectionLabel->setText("Текущее подключение: \nNone");
    }
    if(!fromCallStat)
    {
        if(db.isOpen())
        {
            saveSettings();
            if(ui->driverCombo->currentText() == "QPSQL")
            {
                openDatabaseEDR();
            }
            else
            {
                dataBaseEDR.close();
            }
            return;
        }
        else
        {
            QMessageBox::critical(this,"Error","database not open\n"+db.lastError().text());
            return;
        }
    }
    else fromCallStat = !fromCallStat;

}

void MainWindow::SaveToFile(QString a)
{
    QString fileName;
    a == "Абонент" ? fileName=" Abonent " +abonentLine->text() : a== "Найти IMEI"? fileName = " IMEI "+IMEI->text()+" " : a=="Россия"? fileName = " Russia" : a=="Украина"?
                fileName = " Ukraine": a=="На городские (064)"? fileName = " City (064)" : fileName = " MN";

    if(a=="Абонент")
    {
        ui->incoming->isChecked() ? fileName+=" (" + ui->incoming->text()+")" : ui->outgoing->isChecked() ? fileName +=" ("+ ui->outgoing->text()+")": fileName+=" ("+ui->all_calls->text()+")";
    }

    if(ui->radioButton_2->isChecked())  fileName += "-All time";
    else if(ui->oneDay->isChecked())
    {
        QString date;
        ui->dateEdit_2->date().day()<10 ? date = '0'+QString::number(ui->dateEdit_2->date().day()) : date = QString::number(ui->dateEdit_2->date().day());
        ui->dateEdit_2->date().month()<10 ? date += '0'+QString::number(ui->dateEdit_2->date().month()):date += QString::number(ui->dateEdit_2->date().month());
        date+=QString::number(ui->dateEdit_2->date().year());

        fileName = date + fileName;
    }
    else
    {
        QString date;
        ui->dateEdit_2->date().day()<10 ? date = '0'+QString::number(ui->dateEdit_2->date().day()) : date = QString::number(ui->dateEdit_2->date().day());
        ui->dateEdit_2->date().month()<10 ? date += '0'+QString::number(ui->dateEdit_2->date().month()):date += QString::number(ui->dateEdit_2->date().month());
        date+=QString::number(ui->dateEdit_2->date().year()) + '-';

        ui->dateEdit->date().day()<10 ? date += '0'+QString::number(ui->dateEdit->date().day()) : date += QString::number(ui->dateEdit->date().day());
        ui->dateEdit->date().month()<10 ? date += '0'+QString::number(ui->dateEdit->date().month()):date += QString::number(ui->dateEdit->date().month());
        date+=QString::number(ui->dateEdit->date().year());
        fileName = date + fileName;
    }

    a == "Абонент" ? fileName="Abonent\\"+fileName : a== "Найти IMEI"? fileName = "IMEI\\" +fileName: a=="Россия"? fileName = "RU\\"+fileName : a=="Украина"?
                fileName = "UA\\" +fileName : a=="На городские (064)"? fileName = "CITY\\" +fileName  : fileName = "MN\\" +fileName;

    QString fileNameXLSX = "results\\"+fileName+".xlsx";
    fileName = "results\\"+fileName+".csv";
    if(ui->tabWidget->currentIndex() == 1)
    {
        fileName.replace(".csv","(EDR).csv");
        fileNameXLSX.replace(".xlsx","(EDR).xlsx");
    }
    else
    {
        fileName.replace(".csv","(CDR).csv");
        fileNameXLSX.replace(".xlsx","(CDR).xlsx");
    }

    QXlsx::Document xlsx;
    QFile toFile;
    toFile.setFileName(fileName);
    toFile.open(QIODevice::WriteOnly);

    QTextStream outFile(&toFile);

    QVector<QString> Data;


    QSqlQueryModel *currentModel;
    ui->tabWidget->currentIndex() == 1 ? currentModel = edr_model : ui->tabWidget->currentIndex() == 2 ? currentModel = query_model : currentModel = model;

    if(currentModel->query().isSelect() && currentModel->rowCount())
    {
        QString tmpData;
        for(int i=0;i<currentModel->columnCount();i++)
        {
            i<currentModel->columnCount()-1?tmpData+=currentModel->headerData(i,Qt::Horizontal).toString()+";":tmpData+=currentModel->headerData(i,Qt::Horizontal).toString();
            xlsx.write(1,i+1,currentModel->headerData(i,Qt::Horizontal).toString());
        }

        Data.push_back(tmpData);
        tmpData.clear();

        for(int i=0;i<currentModel->rowCount();i++)
        {
            for(int j=0;j<currentModel->columnCount();j++)
            {
                j<currentModel->columnCount()-1? tmpData+=currentModel->data(currentModel->index(i,j)).toString()+";":tmpData+=currentModel->data(currentModel->index(i,j)).toString();
                xlsx.write(i+2,j+1,currentModel->data(currentModel->index(i,j)).toString());
            }
            Data.push_back(tmpData);
            tmpData.clear();
        }

        for(int i=0;i<Data.size();i++)
            outFile << Data[i] << "\r\n";

        toFile.close();
        xlsx.saveAs(fileNameXLSX);
      QMessageBox::information(this,"Info","Данные сохранены в файлы:\n"+fileName+'\n'+fileNameXLSX);
    }
    else QMessageBox::information(this,"Info","Данных для записи нет");
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if(index==2) ui->saveToFile->setEnabled(false);
    else ui->saveToFile->setEnabled(true);

    if(ui->radioButton->isChecked()) ui->radioButton->setChecked(true);
    if(ui->radioButton_4->isChecked()) ui->radioButton_4->setChecked(true);
    if(ui->radioButton_5->isChecked()) ui->radioButton_5->setChecked(true);
    ui->showData->setFocus();
}

void MainWindow::on_radioButton_clicked()
{

    ui->comboBox->setEnabled(false);
    IMEI->setEnabled(false);

    abonentLine->setEnabled(true);
    ui->outgoing->setEnabled(true);
    ui->incoming->setEnabled(true);
    ui->all_calls->setEnabled(true);
    ui->saveToFile->setEnabled(true);
    abonentLine->setFocus();

}

void MainWindow::on_radioButton_5_clicked()
{
    ui->comboBox->setEnabled(true);
    abonentLine->setDisabled(true);
    ui->outgoing->setDisabled(true);
    ui->all_calls->setDisabled(true);
    ui->incoming->setDisabled(true);
    IMEI->setDisabled(true);
    ui->comboBox->setFocus();
    if(ui->comboBox->currentIndex()==2) ui->saveToFile->setDisabled(true);
}

void MainWindow::on_radioButton_4_clicked()
{
    ui->comboBox->setDisabled(true);
    abonentLine->setDisabled(true);
    ui->outgoing->setDisabled(true);
    ui->all_calls->setDisabled(true);
    ui->incoming->setDisabled(true);
    IMEI->setDisabled(false);
    ui->saveToFile->setEnabled(true);
    IMEI->setFocus();
}

void MainWindow::on_oneDay_clicked()
{
    ui->oneDay->setChecked(true);
    ui->period->setChecked(false);
    ui->radioButton_2->setChecked(false);
    ui->dateEdit->setVisible(false);
    ui->dateEdit_2->setVisible(true);
}

void MainWindow::on_period_clicked()
{
    ui->period->setChecked(true);
    ui->oneDay->setChecked(false);
    ui->radioButton_2->setChecked(false);
    ui->dateEdit->setVisible(true);
    ui->dateEdit_2->setVisible(true);
}

void MainWindow::on_radioButton_2_clicked()
{
    ui->dateEdit->setVisible(false);
    ui->dateEdit_2->setVisible(false);

    ui->radioButton_2->setChecked(true);
    ui->oneDay->setChecked(false);
    ui->period->setChecked(false);
}

void MainWindow::on_pushButton_5_clicked()
{
    if(db.isOpen())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QString request = "select distinct dateForStartOfCharge from cdr order by dateForStartOfCharge";
        if(ui->driverCombo->currentText() == "QPSQL" && ui->tabWidget->currentIndex() == 1)
        {
            QApplication::setOverrideCursor(Qt::ArrowCursor);
            QMessageBox *messageBox = new QMessageBox(this);
            messageBox->setText("Данная операция может занять много времени.\nВы уверены?");
            messageBox->setStandardButtons(QMessageBox::Ok | QMessageBox::No);
            int responce = messageBox->exec();
            if(responce == 1024)
            {
                QApplication::setOverrideCursor(Qt::WaitCursor);
                request = "SELECT DISTINCT datecharge::date as existing_date FROM edr";
                edr_model->setQuery(request,dataBaseEDR);
                ui->tableViewEDR->setModel(edr_model);
                ui->tableViewEDR->resizeColumnsToContents();
                ui->tableViewEDR->resizeColumnsToContents();
                ui->tableViewEDR->resizeRowsToContents();
                ui->tabWidget->setCurrentIndex(1);
                ui->analyseDataButton->setEnabled(false);
                ui->chartButton->setEnabled(false);
                QApplication::setOverrideCursor(Qt::ArrowCursor);
            }

        }
        else if(ui->driverCombo->currentText() == "QPSQL" && ui->tabWidget->currentIndex() == 0)
        {
            QApplication::setOverrideCursor(Qt::ArrowCursor);
            QMessageBox *messageBox = new QMessageBox(this);
            messageBox->setText("Данная операция может занять много времени.\nВы уверены?");
            messageBox->setStandardButtons(QMessageBox::Ok | QMessageBox::No);
            int responce = messageBox->exec();
            if(responce == 1024)
            {
                QApplication::setOverrideCursor(Qt::WaitCursor);
                // cdr date
                request = "SELECT dateforstartofcharge as existing_date FROM mss GROUP BY dateforstartofcharge ORDER BY dateforstartofcharge";
                model->setQuery(request,db);
                ui->tableView->setModel(model);
                ui->tableView->resizeColumnsToContents();
                ui->tableView->resizeColumnsToContents();
                ui->tableView->resizeRowsToContents();
                ui->tabWidget->setCurrentIndex(0);
                ui->analyseDataButton->setEnabled(false);
                ui->chartButton->setEnabled(false);
                QApplication::setOverrideCursor(Qt::ArrowCursor);
            }

        }
        else
        {
            model->setQuery(request,db);
            ui->tableView->setModel(model);
            ui->tableView->resizeColumnsToContents();
            ui->tableView->resizeColumnsToContents();
            ui->tableView->resizeRowsToContents();
            ui->tabWidget->setCurrentIndex(0);
            ui->analyseDataButton->setEnabled(false);
            ui->chartButton->setEnabled(false);
            QApplication::setOverrideCursor(Qt::ArrowCursor);
        }
    }
    else QMessageBox::critical(this,"Ошибка подключения","Откройте БД/Подключитесь к ней");

}

void MainWindow::on_analyseDataButton_clicked()
{
    if(model->rowCount()>0)
    {
        analyse = new AnalyseData(model,lastSelectedAbonent,getDatesToChart());
    }
    else QMessageBox::information(this,"Inforamtion","Нет данных для анализа");
}

void MainWindow::on_pastePB_clicked()
{
    QString paste = QApplication::clipboard()->text();
    if(paste.length()<15)
    {
        if(paste[0]=='3') abonentLine->setText(paste.right(9));
        else abonentLine->setText(paste);
    }
}

void MainWindow::on_cellStat_clicked()
{
    CallStat a;
    a.setDriver(ui->driverCombo->currentText());
    a.exec();
    fromCallStat = true;

}

void MainWindow::abonentAnalyse(int analyseColumn)
{
    if(analyseColumn==-1)
    {
        ui->duration->setVisible(false);
        ui->favoriteNumber->setVisible(false);
        ui->line_2->setVisible(false);
        ui->tableView->setToolTip("");
        return;
    }
    else
    {
        ui->line_2->setVisible(true);
        ui->duration->setVisible(true);
        ui->favoriteNumber->setVisible(true);
    }

    while(model->canFetchMore()) model->fetchMore();

    ui->tableView->setToolTip(calculateFrequnces());
    double temporarySum = 0;
    QVector<QPair<QString,int>> pairVector;
    for(int i=0;i<model->rowCount();i++)
    {
        bool isAlreadyFind = false;
        temporarySum+=model->data(model->index(i,5)).toInt();
        for(int j=0;j<pairVector.size();j++)
        {
            if(pairVector[j].first==model->data(model->index(i,analyseColumn)).toString())
            {
                pairVector[j].second++;
                isAlreadyFind = true;
                break;
            }
        }
        if(!isAlreadyFind) pairVector.push_back(QPair<QString,int>(model->data(model->index(i,analyseColumn)).toString(),1));
    }

    temporarySum /= double(model->rowCount());

    for(int i=0;i<pairVector.size();i++)
        for(int j=0;j<pairVector.size()-1;j++)
            if(pairVector[j].second < pairVector[j+1].second) swap(pairVector[j],pairVector[j+1]);

    if(analyseColumn==2)
    {
        if(pairVector.length()>=3)
            ui->favoriteNumber->setText("Наиболее часто набираемые номера:\n"+pairVector[0].first +" "+QString::number(pairVector[0].second)+"/"+QString::number(model->rowCount())+"\n"+pairVector[1].first+" "
                    +QString::number(pairVector[1].second)+"/"+QString::number(model->rowCount())+"\n"+pairVector[2].first+" "+QString::number(pairVector[2].second)+"/"+QString::number(model->rowCount()));
        else
            ui->favoriteNumber->setText("Наиболее часто набираемый номер: \n"+pairVector[0].first+" "+QString::number(pairVector[0].second)+"/"+QString::number(model->rowCount()));
    }
    else if(analyseColumn==1)
    {
        if(pairVector.length()>=3)
            ui->favoriteNumber->setText("Наиболее часто набирают номер:\n"+pairVector[0].first +" "
                    +QString::number(pairVector[0].second)+"/"+QString::number(model->rowCount())+"\n"+pairVector[1].first+" "+QString::number(pairVector[1].second)
                    +"/"+QString::number(model->rowCount())+"\n"+pairVector[2].first+" "+QString::number(pairVector[2].second)+"/"+QString::number(model->rowCount()));
        else
            ui->favoriteNumber->setText("Наиболее часто набирает номер: \n"+pairVector[0].first+" "+QString::number(pairVector[0].second)+"/"+QString::number(model->rowCount()));
    }

    ui->duration->setText("Длительность разговоров\nв среднем составляет: "+QString::number(temporarySum)+" секунд");
}

void MainWindow::saveSettings()
{
    if(ui->driverCombo->currentText() == "QMYSQL")
    {
        settings->saveAuthData("mainForm/mySqlHost",ui->host->text());
        settings->saveAuthData("mainForm/mySqlPort",ui->port->text());
        settings->saveAuthData("mainForm/mySqlDatabaseName",ui->DBname->text());
        settings->saveAuthData("mainForm/mySqlUserName",ui->login->text());
        settings->saveAuthData("mainForm/mySqlPassword",ui->password->text());
    }
    else if( ui->driverCombo->currentText() == "QPSQL")
    {
        settings->saveAuthData("mainForm/postgresHost",ui->host->text());
        settings->saveAuthData("mainForm/postgresPort",ui->port->text());
        settings->saveAuthData("mainForm/postgresDatabaseName",ui->DBname->text());
        settings->saveAuthData("mainForm/postgresUserName",ui->login->text());
        settings->saveAuthData("mainForm/postgresPassword",ui->password->text());
    }
}

QAbstractItemModel *MainWindow::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(completer);

#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();

        if (!line.isEmpty())
            words << QString::fromUtf8(line.trimmed());
    }

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(words, completer);
}

QString MainWindow::toOneString(QStringList list,QString separator)
{
    QString result;
    for(int i=0;i<list.size();i++)
    {
        result += list[i];
        if(i<list.size()-1) result+=separator;
    }
    return result;
}

void MainWindow::on_driverCombo_currentIndexChanged(QString text)
{

    if(text == "QMYSQL")
    {
        ui->host->setText(settings->getSettigns("mainForm/mySqlHost").toString());
        ui->port->setText(settings->getSettigns("mainForm/mySqlPort").toString());
        ui->DBname->setText(settings->getSettigns("mainForm/mySqlDatabaseName").toString());
        ui->login->setText(settings->getSettigns("mainForm/mySqlUserName").toString());
        ui->password->setText(settings->getSettigns("mainForm/mySqlPassword").toString());
        requestBegin.replace("from mss","from cdr");
        request_MN.replace("from mss","from cdr");
    }
    else if (text == "QPSQL")
    {
        ui->host->setText(settings->getSettigns("mainForm/postgresHost").toString());
        ui->port->setText(settings->getSettigns("mainForm/postgresPort").toString());
        ui->DBname->setText(settings->getSettigns("mainForm/postgresDatabaseName").toString());
        ui->login->setText(settings->getSettigns("mainForm/postgresUserName").toString());
        ui->password->setText(settings->getSettigns("mainForm/postgresPassword").toString());
        requestBegin.replace("from cdr","from mss");
        request_MN.replace("from cdr","from mss");

    }
}

void MainWindow::openDatabaseEDR()
{
    QSqlDatabase::contains("EDR_connection") ? dataBaseEDR = QSqlDatabase::database("EDR_connection") :
            dataBaseEDR  = QSqlDatabase::addDatabase("QPSQL","EDR_connection");
    dataBaseEDR.setPort(settings->getSettigns("mainForm/postgresPort").toInt());
    dataBaseEDR.setHostName(settings->getSettigns("mainForm/postgresHost").toString());
    dataBaseEDR.setDatabaseName("edr");
    dataBaseEDR.setUserName(settings->getSettigns("mainForm/postgresUserName").toString());
    dataBaseEDR.setPassword(settings->getSettigns("mainForm/postgresPassword").toString());
    dataBaseEDR.open();
}

void MainWindow::execAnalyseWithLoadChart()
{
    if(model->rowCount()>0)
    {
        analyse = new AnalyseData(model,lastSelectedAbonent,getDatesToChart(),1);
    }
    else QMessageBox::information(this,"Inforamtion","Нет данных для анализа");
}

void MainWindow::on_chartButton_customContextMenuRequested(const QPoint &pos)
{
    customMenu->popup(ui->chartButton->mapToGlobal(pos));
}

void MainWindow::actionChartButtonTriggered()
{
    QAction *action = qobject_cast<QAction *> (sender());

    if(action == actionDay)
    {
        if(model->rowCount()>0)
        {
            analyse = new AnalyseData(model,lastSelectedAbonent,getDatesToChart(),3);
        }
        else QMessageBox::information(this,"Inforamtion","Нет данных для анализа");
    }
    else if(action == actionWeek)
    {
        if(model->rowCount()>0)
        {
            analyse = new AnalyseData(model,lastSelectedAbonent,getDatesToChart(),7);
        }
        else QMessageBox::information(this,"Inforamtion","Нет данных для анализа");
    }
    else QMessageBox::information(this,"Info","how???");
}

QString MainWindow::calculateFrequnces()
{
    QString result = "Частоты LAC \n";
    int indexLACfirst = 0,indexLACsecond = 0;
    for(int i=0;i<model->columnCount();i++)
    {
        if(model->headerData(i,Qt::Horizontal).toString().toLower() == "firstcallinglac")
            indexLACfirst = i;
        if(model->headerData(i,Qt::Horizontal).toString().toLower() == "lastcallinglac")
            indexLACsecond = i;
    }

    QMap<QString,int> frequences;


    for(int i=0;i<model->rowCount();i++)
    {
        frequences[model->index(i,indexLACfirst).data().toString()]++;
        frequences[model->index(i,indexLACsecond).data().toString()]++;

    }

    for(QMap<QString,int>::iterator it = frequences.begin();it!=frequences.end();++it)
    {
        result += it.key();
        if(it.key()=="62130")
        {
            result+="(Луганск,Славяносербск,Молодогвардейск,Лутугино - 2G)";
        }
        else if(it.key()=="50331")
        {
          result+="(Луганск-3G)";
        }
        else if(it.key()=="62071")
        {
            result+="(Алчевск,Первомайск,Кировск,Брянка,Стаханов - 2G)";
        }
        else if(it.key()=="62626")
        {
            result+="(Свердловск,Ровеньки,Антрацит,Краснопартизанск,Красный Луч - 2G)";
        }
        else if(it.key()=="62500")
        {
            result+="(Краснодон - 2G)";
        }
        else if(it.key()=="50333")
        {
            result+="(Антрацит-3G)";
        }
        else
        {
            result+="(UnknownRegion)";
        }
        result +=':'+QString::number(it.value())+'\n';

    }

    return result;
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QApplication::clipboard()->setText(index.data().toString());
}

