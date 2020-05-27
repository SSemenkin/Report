 #include "analysedata.h"

AnalyseData::AnalyseData(QSqlQueryModel *model, QString abonent, QString dates, int actionValue, QSqlQueryModel *edrModel)
{
    chartsLayout = new QHBoxLayout;
    if(edrModel!=nullptr)
    {
        QMap<QString,int> chartData;
        for(int i =0;i<edrModel->rowCount ();i++){
            if(!edrModel->index (i,10).data ().toString ().isEmpty ())
            chartData[edrModel->index (i,10).data ().toString ()]++;
        }
        if(chartData.size ())
            chartsLayout->addWidget (buildEDRPieChart (chartData));
    }

    settings = new Settings();
    globalLay = new QVBoxLayout;
    hideModel = model;
    currAbon = abonent;
    interval = dates;
    days = abs(actionValue);
    timer = new QTimer();
    label = new QLabel("Получение данных с сервера.");
    dataBar = new QProgressBar();
    threadToSQLDatabase = new QThread();
    updateWindowWidgets();
    if(!actionValue)
    {
        cells = Cells();
        GenerateChart();
    }
    else
    {
        cells = Cells();
        GenerateChart();
    }
}

AnalyseData::~AnalyseData()
{
    threadToSQLDatabase->quit ();
    delete settings;
    delete globalLay;
    delete chartsLayout;
    delete hideModel;
    delete label;
    delete timer;
    delete dataBar;
    delete threadToSQLDatabase;
}

// отрисовка всей формы с графиками и прочим наполнением
void AnalyseData::GenerateChart()
{

    QMenuBar *bar = new QMenuBar(this);
    QMenu *menu = new QMenu("Настройки");
    bar->addMenu(menu);
    QAction *action = new QAction("Настройки подключения к BSC",this);
    menu->addAction(action);
    connect(action,&QAction::triggered,this,&AnalyseData::showSettingsWidget);
    globalLay->addWidget(bar);
    globalLay->addLayout(chartsLayout);

    lb = new QTextEdit;
    lb->setFixedHeight(100);
    QFont font;
    font.setBold(true);
    font.setPixelSize(14);
    this->setFont(font);

    if(chastoti.size()>0)
    {
        chartsLayout->addWidget(buildPieChart());
    }
    if(days!=0)
    {
            globalLay->addWidget(label);
            globalLay->addWidget(dataBar);
            timer->setInterval(1000);
            connect(timer,&QTimer::timeout,this,&AnalyseData::changeText);

            timer->start();
            sepChartClass = new ChartBySeparateThread();
            sepChartClass->cells = cells;
            sepChartClass->days = days;

            sepChartClass->moveToThread(threadToSQLDatabase);
            if(QSqlDatabase::contains ("QMYSQL_detail_connection")){
                QSqlDatabase::removeDatabase ("QMYSQL_detail_connection");
            }
            connect(threadToSQLDatabase,&QThread::started,sepChartClass,&ChartBySeparateThread::getChargeOf2GCellsFromMySQL);
            connect(sepChartClass,&ChartBySeparateThread::dataRecieved,dataBar,&QProgressBar::setValue);
            connect(sepChartClass,&ChartBySeparateThread::finished,this,&AnalyseData::storeResult);
            threadToSQLDatabase->start();
    }



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
            if(chastoti[i].first[0]!='(')
                lb->setText(lb->toPlainText()+"rlcrp:cell="+chastoti[i].first+";\n");
    }
    if(ch3G->isChecked())
    {
        for(int i=0;i<chastoti.size();i++)
            if(chastoti[i].first[0]=='(')
                lb->setText(lb->toPlainText()+"DSP UCELLCHK: CHECKSCOPE=CELLID, CellId="+chastoti[i].first.right(5)+";\n");
    }
}

void AnalyseData::showSettingsWidget()
{
    Settings a;
    auto result = a.exec();
    if(result == QDialog::Accepted)
    {
        for(int i=0;i<customSlices.size();i++)
            customSlices[i]->loadSettings();
    }
}

void AnalyseData::ShowLoad(QString cell)
{
    if(cell[0]=='L')
     for(int i=0;i<loadSeriesV.size();i++)
     {
         if(loadSeriesV[i]->name() == cell)
         {
             loadSeriesV[i]->setOpacity(1);
             interSeries[i]->setOpacity(1);
         }
         else
         {
             interSeries[i]->setOpacity(0.2);
             loadSeriesV[i]->setOpacity(0.2);
         }
     }
}

QChartView *AnalyseData::buildPieChart()
{
    QFont font;
    font.setBold(true);
    font.setPixelSize(14);

    QChart *pieChart = new QChart;
    pieChart->legend()->setAlignment(Qt::AlignRight);
    pieChart->setTitle("Частотный анализ секторов (CDR) ");
    pieChart->setAnimationOptions(QChart::AllAnimations);


    QPieSeries *pieSeries = new QPieSeries();
    int Size = 0;
    for(int i=0;i<chastoti.size();i++)
        Size+=chastoti[i].second;


    for(int i=0;i<chastoti.size();i++)
        for(int j=0;j<chastoti.size()-1;j++)
            if(chastoti[j].second<chastoti[j+1].second) swap(chastoti[j],chastoti[j+1]);


    CustomSlice *slice;

    for(int i=0;i<chastoti.size() && i < 20;i++)
    {
        slice = new CustomSlice(chastoti[i].first,chastoti[i].second);
        customSlices.push_back(slice);
        connect(slice,&CustomSlice::sliceHovered,this,&AnalyseData::ShowLoad);
        slice->setLabelFont(font);
        if((double(chastoti[i].second) / double(Size) * 100.0) < 5.0 )
            slice->setLabelPosition(QPieSlice::LabelOutside);
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

    return chartView;
}

QChartView *AnalyseData::buildEDRPieChart(QMap<QString,int> chartData)
{
    QFont font;
    font.setBold(true);
    font.setPixelSize(14);

    QChart *pieChart = new QChart;
    pieChart->legend()->setAlignment(Qt::AlignRight);
    pieChart->setTitle("Частотный анализ секторов (EDR) ");
    pieChart->setAnimationOptions(QChart::AllAnimations);


    QPieSeries *pieSeries = new QPieSeries();


    CustomSlice *slice;

    for(auto it = chartData.begin ();it!=chartData.end ();++it){
        slice = new CustomSlice(it.key (),chartData[it.key ()]);

        slice->setLabelFont(font);
        slice->setLabelPosition (CustomSlice::LabelPosition::LabelOutside);
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

    return chartView;
}

QTabWidget *AnalyseData::buildLineCharts(QVector<cell2G> cells2G)
{
    QFont font;
    font.setBold(true);
    font.setPixelSize(14);

    QTabWidget *tabWidget = new QTabWidget();
    QChart *icmChart = new QChart();
    QChart *chargeChart = new QChart();
    chargeChart->setFont(font);
    chargeChart->setTitleFont(font);
    chargeChart->legend()->setFont(font);
    icmChart->setFont(font);
    icmChart->setTitleFont(font);
    icmChart->legend()->setFont(font);

    if(days==1)
        chargeChart->setTitle("График нагрузки на сектора 2G за последние сутки,\nс которых звонил абонент");
    else chargeChart->setTitle("График нагрузки на сектора 2G за последние "+QString::number(abs(days))+ " суток,\nс которых звонил абонент");
    chargeChart->setAnimationOptions(QChart::SeriesAnimations);
    chargeChart->legend()->setAlignment(Qt::AlignRight);
    icmChart->setTitle("Интерференция на данных секторах");
    icmChart->setAnimationOptions(QChart::AllAnimations);
    icmChart->legend()->setAlignment(Qt::AlignRight);

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTitleText("Время");
    axisX->setTickCount(10);
    if(days==1) axisX->setFormat("hh:mm");
    else axisX->setFormat("dd-MM hh:mm");
    chargeChart->addAxis(axisX,Qt::AlignBottom);

    QDateTimeAxis *axisXi = new QDateTimeAxis;
    axisXi->setTitleText("Время");
    axisXi->setTickCount(10);
    if(days==1) axisXi->setFormat("hh:mm");
    else axisXi->setFormat("dd-MM hh:mm");
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
         loadSeriesV.push_back(tmpSeries);
         interSeries.push_back(tmpSeriesi);
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
    tabWidget->addTab(chartViewi,"Интерференция");
    return tabWidget;
}

void AnalyseData::storeResult()
{
    returnedData = sepChartClass->returneD;
    repaintModel();
    emit threadFinished (true);
}

void AnalyseData::repaintModel()
{
    threadToSQLDatabase->quit();
    delete sepChartClass;
    timer->stop();
    label->setVisible(false);
    dataBar->setVisible(false);
    chartsLayout->addWidget(buildLineCharts(returnedData));
}

void AnalyseData::changeText()
{
    if(label->text().right(3) == "ра.")
    {
        label->setText(label->text()+'.');
    }
    else if(label->text().right(3) == "а..")
    {
        label->setText(label->text()+'.');
    }
    else
    {
       label->setText(label->text().left(label->text().length()-2));
    }
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
