#include "analysedata.h"

AnalyseData::~AnalyseData()
{
    if(mysqldataBase.isOpen())
        mysqldataBase.close();
}

AnalyseData::AnalyseData(QSqlQueryModel *model,QString abonent,QString dates)
{
    settings = new Settings();
    globalLay = new QVBoxLayout;
    chartsLayout = new QHBoxLayout;
    hideModel = model;
    currAbon = abonent;
    interval = dates;
    updateWindowWidgets();
    GenerateChart(getChargeOf2GCellsFromMySQL(Cells()));

}
// отрисовка всей формы с графиками и прочим наполнением
void AnalyseData::GenerateChart(QVector<cell2G> cells2G)
{

    lb = new QTextEdit;
    lb->setFixedHeight(100);
    QFont font;
    font.setBold(true);
    font.setPixelSize(14);
    this->setFont(font);

    if(chastoti.size()>0)
    {
        QChart *pieChart = new QChart;
        pieChart->legend()->setAlignment(Qt::AlignRight);
        pieChart->setTitle("Частотный анализ секторов");
        pieChart->setAnimationOptions(QChart::AllAnimations);


        QPieSeries *pieSeries = new QPieSeries();
        int Size = 0;
        for(int i=0;i<chastoti.size();i++)
            Size+=chastoti[i].second;


        for(int i=0;i<chastoti.size();i++)
            for(int j=0;j<chastoti.size()-1;j++)
                if(chastoti[j].second<chastoti[j+1].second) swap(chastoti[j],chastoti[j+1]);


        CustomSlice *slice;
        int value = chastoti.size();
        if(chastoti.size()>20) value  = 20;
        for(int i=0;i<value;i++)
        {
            slice = new CustomSlice(chastoti[i].first,chastoti[i].second);
            slice->setLabelFont(font);
            *pieSeries << slice;

        }
        pieChart->addSeries(pieSeries);
        pieSeries->setLabelsVisible(true);

        QChartView *chartView = new QChartView();
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setChart(pieChart);
        pieChart->setTitleFont(font);
        pieChart->legend()->setFont(font);
        pieChart->setFont(font);

        chartsLayout->addWidget(chartView);

    }
    if(cells2G.size()>0)
    {
        QTabWidget *tabWidget = new QTabWidget();
        QChart *icmChart = new QChart();
        QChart *chargeChart = new QChart();
        chargeChart->setFont(font);
        chargeChart->setTitleFont(font);
        chargeChart->legend()->setFont(font);
        icmChart->setFont(font);
        icmChart->setTitleFont(font);
        icmChart->legend()->setFont(font);

        chargeChart->setTitle("График нагрузки на сектора 2G за последние сутки,\nс которых звонил абонент");
        chargeChart->setAnimationOptions(QChart::SeriesAnimations);
        chargeChart->legend()->setAlignment(Qt::AlignRight);
        icmChart->setTitle("Интерференция на данных секторах");
        icmChart->setAnimationOptions(QChart::SeriesAnimations);
        icmChart->legend()->setAlignment(Qt::AlignRight);

        QDateTimeAxis *axisX = new QDateTimeAxis;
        axisX->setTitleText("Время");
        axisX->setTickCount(10);
        axisX->setFormat("hh:mm");
        chargeChart->addAxis(axisX,Qt::AlignBottom);

        QDateTimeAxis *axisXi = new QDateTimeAxis;
        axisXi->setTitleText("Время");
        axisXi->setTickCount(10);
        axisXi->setFormat("hh:mm");
        icmChart->addAxis(axisXi,Qt::AlignBottom);

        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("Загрузка(%)");
        axisY->setRange(0,100);
        chargeChart->addAxis(axisY, Qt::AlignLeft);

        QValueAxis *axisYi = new QValueAxis();
        axisYi->setTitleText("Интерференция");
        axisYi->setRange(0,5);
        axisYi->setTickCount(6);
        icmChart->addAxis(axisYi, Qt::AlignLeft);

        for(int i=0;i<cells2G.size();i++)
        {
             QLineSeries *tmpSeries = new QLineSeries();
             tmpSeries->setName(cells2G[i].cellName);
             QLineSeries *tmpSeriesi = new QLineSeries();
             tmpSeriesi->setName(cells2G[i].cellName);
             for(int j=0;j<cells2G[i].dates.size();j++)
             {
                 tmpSeries->append(cells2G[i].dates[j].toMSecsSinceEpoch(),cells2G[i].charges[j]);
                 tmpSeriesi->append(cells2G[i].dates[j].toMSecsSinceEpoch(),cells2G[i].inter[j]);
             }
             chargeChart->addSeries(tmpSeries);
             icmChart->addSeries(tmpSeriesi);
             tmpSeries->attachAxis(axisX);
             tmpSeries->attachAxis(axisY);
             tmpSeriesi->attachAxis(axisXi);
             tmpSeriesi->attachAxis(axisYi);
        }

        QChartView *chartView = new QChartView(chargeChart);
        chartView->setRenderHint(QPainter::Antialiasing);
        QChartView *chartViewi = new QChartView(icmChart);
        chartViewi->setRenderHint(QPainter::Antialiasing);
        tabWidget->addTab(chartView,"Загрузка");
        tabWidget->addTab(chartViewi,"Интерфернция");
        chartsLayout->addWidget(tabWidget);
    }

    QMenuBar *bar = new QMenuBar(this);
    QMenu *menu = new QMenu("Настройки");
    bar->addMenu(menu);
    QAction *action = new QAction("Настройки подключения к BSC",this);
    menu->addAction(action);
    connect(action,&QAction::triggered,this,&AnalyseData::showSettingsWidget);
    globalLay->addWidget(bar);
    globalLay->addLayout(chartsLayout);

    QHBoxLayout  *hbox = new QHBoxLayout;

    QPushButton *pb = new QPushButton;

    pb->setIcon(QIcon(":/images/xCopy.png"));
    pb->setText("Copy Commands");
    pb->setAttribute(Qt::WA_TranslucentBackground);

    connect(pb,&QPushButton::clicked,this,&AnalyseData::xCopy);
    QVBoxLayout *vbox = new QVBoxLayout;
    ch2G = new QCheckBox;
    ch2G->setText("Добавить команды для сети 2G");
    ch3G = new QCheckBox;
    ch3G->setText("Добавить команды для сети 3G");

    connect(ch2G,&QCheckBox::stateChanged,this,&AnalyseData::setCommands);
    connect(ch3G,&QCheckBox::stateChanged,this,&AnalyseData::setCommands);

    vbox->addWidget(ch2G);
    vbox->addWidget(ch3G);
    vbox->addWidget(pb);

    hbox->addWidget(lb);
    hbox->addLayout(vbox);

    globalLay->addLayout(hbox);

    pb->setStyleSheet("background-color:white;"
                         "color:black");


    globalLay->setAlignment(pb,Qt::AlignBottom);


    this->setLayout(globalLay);
    this->showMaximized();
}
//наполнение командами для просмотра нагрузки
void AnalyseData::setCommands(int value)
{
    Q_UNUSED(value);
    lb->clear();

    if(ch2G->isChecked())
    {
        for(int i=0;i<chastoti.size();i++)
            if(chastoti[i].first[0]!='3')
                lb->setText(lb->toPlainText()+"rlcrp:cell="+chastoti[i].first+";\n");
    }
    if(ch3G->isChecked())
    {
        for(int i=0;i<chastoti.size();i++)
            if(chastoti[i].first[0]=='3')
                lb->setText(lb->toPlainText()+"DSP UCELLCHK: CHECKSCOPE=CELLID, CellId="+chastoti[i].first.right(7).left(5)+";\n");
    }
}

void AnalyseData::showSettingsWidget()
{
    Settings a;
    a.exec();
}

QVector<cell2G> AnalyseData::getChargeOf2GCellsFromMySQL(QVector<QString> cells)
{
    QVector<cell2G> result(cells.size());

    if(openLocalDataBase())
    {
        QSqlQueryModel *chargeModel = new QSqlQueryModel;
        QString query;
        for(int i=0;i<cells.size();i++)
        {
            query = "select TCH,SPEECH,GPRS,TIME,INTERFERENCE from bscrecords where RBS='"+cells[i].left(6)+"' and CELLNAME = '"+cells[i].right(1)+"' and TIME between '"+QDateTime::currentDateTime().addDays(-1).toString(Qt::ISODate)
                    +"' and '"+QDateTime::currentDateTime().toString(Qt::ISODate)+"'";
            chargeModel->setQuery(query,mysqldataBase);
            cell2G tmp;
            tmp.cellName = cells[i];
            for(int j=0;j<chargeModel->rowCount();j++)
            {
                tmp.dates.push_back(QDateTime::fromString(chargeModel->data(chargeModel->index(j,3)).toString(),Qt::ISODate));
                tmp.charges.push_back(((chargeModel->index(j,1).data().toFloat()+chargeModel->index(j,2).data().toFloat()) / chargeModel->index(j,0).data().toFloat())*100.0);
                tmp.inter.push_back(chargeModel->index(j,4).data().toInt());
            }
            result[i] = tmp;
        }
        delete chargeModel;
        mysqldataBase.close();
    }

    return result;
}

void AnalyseData::xCopy(bool b)
{
    Q_UNUSED(b);
    QApplication::clipboard()->setText(lb->toPlainText());
}

void AnalyseData::swap(QPair<QString,int> &a,QPair<QString,int> &b)
{
    QPair<QString,int> tmp;
    tmp = a;
    a = b;
    b = tmp;
}

bool AnalyseData::openLocalDataBase()
{
    QSqlDatabase::contains("QMYSQL_detail_connection") ? mysqldataBase = QSqlDatabase::database("QMYSQL_detail_connection") :
            mysqldataBase = QSqlDatabase::addDatabase("QMYSQL","QMYSQL_detail_connection");
    mysqldataBase.setPort(settings->getSettigns("mainForm/mySqlPort").toInt());
    mysqldataBase.setHostName(settings->getSettigns("mainForm/mySqlHost").toString());
    mysqldataBase.setDatabaseName(settings->getSettigns("mainForm/mysqlDatabaseName").toString());
    mysqldataBase.setUserName(settings->getSettigns("mainForm/mySqlUserName").toString());
    mysqldataBase.setPassword(settings->getSettigns("mainForm/mySqlPassword").toString());
    mysqldataBase.open();
    return mysqldataBase.isOpen();
}

void AnalyseData::updateWindowWidgets()
{

    this->setWindowTitle("Исходящие абонента "+currAbon);
    this->setWindowIcon(QIcon(":/images/detail.png"));
}

// метод заполняет частоты появления секторов, а также отделяет сектора 2г в отдельный вектор results
QVector<QString> AnalyseData::Cells()
{
    QVector<QString> result;
    while(hideModel->canFetchMore()) hideModel->fetchMore();

    int indexOfCell = 0,indexOfLAC = 0;
    for(int i=0;i<hideModel->columnCount();i++)
    {
        if(hideModel->headerData(i,Qt::Horizontal).toString().toLower()=="firstcallingcellid" && indexOfCell ==0)
            indexOfCell = i;
        if(hideModel->headerData(i,Qt::Horizontal).toString().toLower()=="firstcallinglac" && indexOfLAC == 0)
            indexOfLAC = i;
    }

    for(int i=0;i<hideModel->rowCount();i++)
    {
        if(hideModel->data(hideModel->index(i,indexOfLAC)).toString()!="50331" && hideModel->data(hideModel->index(i,indexOfLAC)).toString()!="50333")
        {

            QString sec;
            QString tmpName = hideModel->data(hideModel->index(i,indexOfCell)).toString();
            tmpName.right(1)=='1'? sec = 'A' : tmpName.right(1)=='2'? sec='B':sec='C';
            tmpName = tmpName.left(tmpName.length()-1)+sec;
            if(tmpName.length()==2)
                tmpName = "00" + tmpName;
            if(tmpName.length()==3)
                tmpName='0'+tmpName;
            tmpName.length()==5?tmpName = "LU"+tmpName:tmpName = "LUG"+tmpName;

            bool was = false;
            for(int j=0;j<chastoti.size();j++)
            {
                if(chastoti[j].first==tmpName)
                {
                    chastoti[j].second++;
                    was = true;
                    break;
                }
            }
            if(!was) chastoti.push_back(QPair<QString,int>(tmpName,1));

            bool is_find = false;
            for(int j=0;j<result.size();j++)
            {
                if(result[j]==tmpName) is_find = true;
            }

            if(!is_find) result.push_back(tmpName);
        }
        else {
            QString cell = "(3G)"+ hideModel->data(hideModel->index(i,9)).toString();
            bool was = false;
            for(int j=0;j<chastoti.size();j++)
            {
                if(chastoti[j].first==cell)
                {
                    chastoti[j].second++;
                    was = true;
                    break;
                }
            }
            if(!was)
            {
                chastoti.push_back(QPair<QString,int>(cell,1));
            }
        }

    }
    return result;
}
