#include "analysedata.h"


AnalyseData::AnalyseData()
{

}

AnalyseData::~AnalyseData()
{
    dataBase.close();
}

AnalyseData::AnalyseData(QSqlQueryModel *model,QString abonent,QString dates)
{

    globalLay = new QVBoxLayout;
    hideModel = model;
    currAbon = abonent;
    interval = dates;
    updateWindowWidgets();
    QVector<QString> cellFind = Cells();

    if(openLocalDataBase())
    {
        QVector<QVector<int>> loadData = prepareData(7,cellFind);
        QVector<QVector<int>> icmBandData = prepareData(6,cellFind);
        GenerateChart(loadData,icmBandData,getDateList(),cellFind);
    }
    //else exit(1);
}

void AnalyseData::GenerateChart(QVector<QVector<int>> loadCell
                                ,QVector<QVector<int>> icmBand,QVector<QString> dateList,QVector<QString> cellNames)
{
    lb = new QTextEdit;
    lb->setFixedHeight(100);
    QFont font;
    font.setBold(true);
    font.setPixelSize(14);

    if(chastoti.size()!=0)
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
            slice = new CustomSlice(chastoti[i].first+'('+QString::number(double(double(chastoti[i].second)/double(Size)*100))+"%)",chastoti[i].second);
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
        chartView->setFixedHeight(700);

        QMenuBar *bar = new QMenuBar(this);
        QMenu *menu = new QMenu("File");
        bar->addMenu(menu);
        QAction *action = new QAction("Settings",this);
        menu->addAction(action);

        connect(action,&QAction::triggered,this,&AnalyseData::showSettingsWidget);


        globalLay->addWidget(bar);

        globalLay->addWidget(chartView);

    }

    if(dateList.size()!=0)
    {

        QChart *loadChart = new QChart;
        QChart *icmChart = new QChart;
        loadChart->setFont(font);
        icmChart->setFont(font);
        loadChart->setTitleFont(font);
        icmChart->setTitleFont(font);
        loadChart->legend()->setFont(font);
        icmChart->legend()->setFont(font);

        loadChart->setTitle("График загрузки базовых станций");
        QDateTimeAxis *axisX = new QDateTimeAxis;
        QDateTimeAxis *axisXband = new QDateTimeAxis;

        QValueAxis *axisY = new QValueAxis;
        QValueAxis *axisYband = new QValueAxis;
        axisY->setRange(0,100);
        axisYband->setRange(0,6);
        axisYband->setTickCount(7);


        for(int i=0;i<dateList.size();i++)
        {
            for(int j=0;j<loadCell.size();j++)
            {
                QLineSeries *loadSeries = new QLineSeries;
                QLineSeries *icmSeries = new QLineSeries;

                loadSeries->setName(cellNames[i]);

                loadSeries->setPointsVisible();
                icmSeries->setPointsVisible();
                for(int k=0;k<loadCell[j].size();k++)
                {
                    QStringList a = dateList[i].split(';');
                    QStringList tmpDate = a[0].split('-');
                    QStringList tmpTime = a[1].split(':');
                    QDateTime pointInTime;
                    pointInTime.setDate(QDate(tmpDate[0].toInt(),tmpDate[1].toInt(),tmpDate[2].toInt()));
                    pointInTime.setTime(QTime(tmpTime[0].toInt(),tmpTime[1].toInt()));

                    loadSeries->append(pointInTime.toMSecsSinceEpoch(),loadCell[j][k]);
                    icmSeries->append(pointInTime.toMSecsSinceEpoch(),icmBand[j][k]);
                }
                loadChart->addSeries(loadSeries);
                icmChart->addSeries(icmSeries);

                loadSeries->attachAxis(axisY);
                loadSeries->attachAxis(axisX);

                icmSeries->attachAxis(axisYband);
                icmSeries->attachAxis(axisXband);

                loadChart->update();
                icmChart->update();

            }
        }

        axisX->setFormat("dd MMM h:mm");
        axisXband->setFormat("dd MMM h:mm");

        loadChart->setTitle("Загрузка БС");
        icmChart->setTitle("Интерференция БС");

        QChartView *loadView = new QChartView(loadChart);
        QChartView *icmView = new QChartView(icmChart);

        loadChart->addAxis(axisY,Qt::AlignLeft);
        loadChart->addAxis(axisX,Qt::AlignBottom);
        icmChart->addAxis(axisYband,Qt::AlignLeft);
        icmChart->addAxis(axisXband,Qt::AlignBottom);

        loadChart->setAnimationOptions(QChart::AllAnimations);
        icmChart->setAnimationOptions(QChart::AllAnimations);

        loadView->setRenderHint(QPainter::Antialiasing);
        icmView->setRenderHint(QPainter::Antialiasing);

        globalLay->addWidget(loadView);
        globalLay->addWidget(icmView);

    }
    else
    {
        //lb->setText("\nК сожалению данных о нагрузке нет");


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



       globalLay->addLayout(hbox);
       globalLay->setAlignment(pb,Qt::AlignBottom);

       globalLay->addWidget(getCompleteListCell());



    }
    this->setLayout(globalLay);
    this->resize(1400,800);
}

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

void AnalyseData::xCopy(bool b)
{
    Q_UNUSED(b);
    QApplication::clipboard()->setText(lb->toPlainText());
}

QVector<QString> AnalyseData::getDateList()
{
    QVector<QString> result;
    QSqlQueryModel *tmpModel = new QSqlQueryModel;
    QString tmpRequest = "SELECT distinct records.date_charge, records.time_charge from (stats inner join records on stats.id_time=records.ID) inner join geographic on stats.RBS=geographic.cell_name"
                         " where "+interval.right(interval.length()-4)+" order by records.date_charge;";
    tmpModel->setQuery(tmpRequest,dataBase);
    while(tmpModel->canFetchMore()) tmpModel->fetchMore();

    for(int i=0;i<tmpModel->rowCount();i++)
        result.push_back( tmpModel->data(tmpModel->index(i,0)).toString() +';'+tmpModel->data(tmpModel->index(i,1)).toString());
    return result;
}

void AnalyseData::swap(QPair<QString,int> &a,QPair<QString,int> &b)
{
    QPair<QString,int> tmp;
    tmp = a;
    a = b;
    b = tmp;
}

QVector<QColor> AnalyseData::getPalitra()
{
    QVector<QColor> resultData;
    QFile file;
    file.setFileName("colors.txt");
    file.open(QIODevice::ReadOnly);
    if(file.isOpen())
    {
        QString buffer;
        while(!file.atEnd())
        {
            buffer=file.readLine();
            if(buffer[0]!='#')
            {
                QStringList a = buffer.split(',');
                resultData.push_back(QColor(a[0].toInt(),a[1].toInt(),a[2].toInt()));
            }
        }
    }

    if(resultData.size()==0) resultData.push_back(QColor(0,0,0));

    return resultData;
}

bool AnalyseData::openLocalDataBase()
{
    QSqlDatabase::contains("chart_connection") ? dataBase = QSqlDatabase::database("chart_connection"):
            dataBase = QSqlDatabase::addDatabase("QMYSQL","chart_connection");
    dataBase.setPort(dataBasePort);
    dataBase.setHostName(dataBaseHost);
    dataBase.setDatabaseName(dataBaseName);
    dataBase.setUserName(dataBaseUser);
    dataBase.setPassword(dataBasePassword);
    dataBase.open();
    return dataBase.isOpen();
}

void AnalyseData::updateWindowWidgets()
{

    this->setWindowTitle("Детальная информация об абоненте "+currAbon);
    this->setWindowIcon(QIcon(":/images/detail.png"));
}

QVector<QVector<int>> AnalyseData::prepareData(int columnIndex,QVector<QString> findCells)
{
    QVector<QVector<int>> Data;

    QString requestModel = "SELECT * from (stats inner join records on stats.id_time=records.ID) inner join geographic on stats.RBS=geographic.cell_name";

    QSqlQueryModel *tmpModel = new QSqlQueryModel;


    for(int i=0;i<findCells.size();i++)
    {
        QString tmpRequest =requestModel + " where stats.RBS='"+findCells[i].left(6)+
                "' and stats.cell_name = '"+findCells[i].right(1)+"' "+interval+" order by records.date_charge;";
        tmpModel->setQuery(tmpRequest,dataBase);
        while(tmpModel->canFetchMore()) tmpModel->fetchMore();

        QVector<int> localDataCell;

        for(int j=0;j<tmpModel->rowCount();j++)
        {
            localDataCell.push_back(tmpModel->data(tmpModel->index(j,columnIndex)).toInt());
        }

        Data.push_back(localDataCell);
    }
    return Data;
}

QVector<QString> AnalyseData::Cells()
{
    QVector<QString> result;
    while(hideModel->canFetchMore()) hideModel->fetchMore();

    int indexOfCell = 0,indexOfLAC = 0;
    for(int i=0;i<hideModel->columnCount();i++)
    {
        if(hideModel->headerData(i,Qt::Horizontal).toString()=="firstCallingCellID")
            indexOfCell = i;
        if(hideModel->headerData(i,Qt::Horizontal).toString()=="firstCallingLAC")
            indexOfLAC = i;
    }

    for(int i=0;i<hideModel->rowCount();i++)
    {
        if(hideModel->data(hideModel->index(i,indexOfLAC)).toString()!="50331")
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
            QString cell = "3G(IdCell = "+ hideModel->data(hideModel->index(i,9)).toString()+" )";
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
            if(!was) chastoti.push_back(QPair<QString,int>(cell,1));
        }
    }
    return result;
}

QTextBrowser* AnalyseData::getCompleteListCell()
{
    QStringList listCell;
    QFile file;
    file.setFileName("base.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream inFile(&file);
    if(file.isOpen())
    {
        QString buffer;
        while(!inFile.atEnd())
        {
            buffer = inFile.readLine();
            for(int i=0;i<chastoti.size();i++)
            {
                QString normalCellName;
                chastoti[i].first[0]=='3'? normalCellName = chastoti[i].first.right(7).left(5)
                        : normalCellName = chastoti[i].first.left(6);
                if(normalCellName==buffer.left(normalCellName.length()))
                {
                    listCell.push_back(buffer);
                    break;
                }
            }
        }
    }
    file.close();

    QStringList tmp = listCell;
    listCell.clear();

    for(int i=0;i<chastoti.size();i++)
        for(int j=0;j<tmp.length();j++)
            if(chastoti[i].first[0]=='3')
            {
                if(chastoti[i].first.right(7).left(5)==tmp[j].left(5)) listCell.push_back(tmp[j]);
            }
            else
            {
                if(chastoti[i].first.left(6)==tmp[j].left(6)) listCell.push_back(tmp[j]);
            }

    QTextBrowser *tb = new QTextBrowser;
    QFont font;
    font.setPixelSize(16);
    tb->setFont(font);
    tb->setAlignment(Qt::AlignCenter);
    for(int i=0;i<listCell.size();i++)
        tb->setText(tb->toPlainText()+listCell[i]+"\n");
    return tb;
}


