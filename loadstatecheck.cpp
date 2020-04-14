#include "loadstatecheck.h"

LoadStateCheck::LoadStateCheck(QString str)
{
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    this->setWindowIcon(QIcon(":/images/haisenberg.png"));
    if(str=="tmp") return;
    this->setWindowTitle("Загрузка соты "+str);
    int SpeechCounter,GPRSCounter,all;
    SpeechCounter = GPRSCounter = all = 0;
    QFile file;
    file.setFileName(str+".txt");
    file.open(QIODevice::ReadOnly);
    QString buffer;
    QStringList allTextCell;
    bool is_begin = false,is_msp=false;
    while(!file.atEnd())
    {
        buffer = file.readLine();
        if(subString(buffer,"BPC   CHANNEL      CHRATE  SPV    STATE"))
        {
            is_begin = true;
        }
        if(subString(buffer,"MO               STATE  BLSTATE "))
        {
            is_begin = false;
            is_msp = true;
        }
        if(is_begin) allTextCell.push_back(buffer);
        if(buffer.left(2)=="LU")
        {
            for(int i=0;i<buffer.length();i++)
            {
                if(buffer[i]=='-')
                {
                    QString number;
                    for(int j=i+1;j<i+4;j++)
                    {
                        number+=buffer[j];
                    }
                    all = number.toInt();
                }
            }
            for(int i=0;;i++)
            {
                buffer = file.readLine();
                allTextCell.push_back(buffer);
                if(subString(buffer,"SPEECH")) SpeechCounter++;
                if(subString(buffer," GPRS")) GPRSCounter++;
                if(subString(buffer,"END")) break;
            }
        }
    }
    file.close();
    file.remove();

    file.setFileName(str+"-msp.txt");
    file.open(QIODevice::ReadOnly);
    QString msp;
    while(!file.atEnd())
    {
        buffer = file.readLine();
        msp+=buffer;
        if(subString(buffer,"END")) break;
    }
    file.close();
    file.remove();
    makeModel(makeTextBeautiful(allTextCell),msp,all,SpeechCounter,GPRSCounter);

}

bool LoadStateCheck::subString(QString buff, QString findStr)
{
    for(int i=0;i<buff.length()-(findStr.length()-1);i++)
    {
        QString substring;
        for(int j=0;j<findStr.length();j++)
            substring+=buff[i+j];
        if(substring==findStr) return true;
    }
    return false;
}

LoadStateCheck::~LoadStateCheck()
{
    delete this->layout();
}

void LoadStateCheck::makeModel( QStringList allTextCell,QString msp ,int all, int speech, int gprs)
{
    QProgressBar *bar = new QProgressBar;
    bar->setAlignment(Qt::AlignHCenter);
    bar->setRange(0,100);


    int value = int(double((double(speech)+double(gprs))/double(all))*100);
    if(value>100)
        value = 100;

    if(value<=10)  bar->setStyleSheet("QProgressBar::chunk {"
                                      "background-color:#D3D3D3;"
                                      "border-radius:5px;"
                                      "}"
                                      "QProgressBar{"
                                      "border-radius:5px;"
                                      "border: 1px solid grey;"
                                      "}");
    else if (value<=50 && value>10) bar->setStyleSheet("QProgressBar::chunk {"
                                      "background-color:#ADFF2F;"
                                      "border-radius:5px;"
                                      "}"
                                      "QProgressBar{"
                                      "border-radius:5px;"
                                      "border: 1px solid grey;"
                                      "}");
    else if (value>50 && value<=80) bar->setStyleSheet("QProgressBar::chunk {"
                                                       "background-color:#FFD700;"
                                                       "border-radius:5px;"
                                                       "}"
                                                       "QProgressBar{"
                                                       "border-radius:5px;"
                                                       "border: 1px solid grey;"
                                                       "}");
    else bar->setStyleSheet("QProgressBar::chunk {"
                            "background-color:red;"
                            "border-radius:5px;"
                            "}"
                            "QProgressBar{"
                            "border-radius:5px;"
                            "border: 1px solid grey;"
                            "}");

    bar->setValue(value);

    QTextBrowser *cellBrowser = new QTextBrowser;
    for(int i=0;i<allTextCell.size();i++)
        cellBrowser->setText(cellBrowser->toPlainText()+allTextCell[i]);

    QTextBrowser *mspBrowser = new QTextBrowser;
    mspBrowser->setText(msp);

    QLabel *lb = new QLabel;
    lb->setText("Общее количество слотов составляет: "+QString::number(all));

    QLabel *lb2 = new QLabel;
    lb2->setText("Количество слотов занятых под SPEECH: "+QString::number(speech));

    QLabel *lb3 = new QLabel;
    lb3->setText("Количество слотов занятых под GPRS: "+QString::number(gprs));

    QLabel *lbMain = new QLabel;
    lbMain->setText("Загрузка");
    lbMain->setAlignment(Qt::AlignCenter);
    QFont font;
    font.setPixelSize(14);
    lbMain->setFont(font);

    QVBoxLayout *VBox = new QVBoxLayout;
    VBox->addWidget(lbMain);
    VBox->addWidget(bar);
    VBox->addWidget(lb);
    VBox->addWidget(lb2);
    VBox->addWidget(lb3);

    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->addTab(mspBrowser,"Состояние железа");
    tabWidget->addTab(cellBrowser,"Состояние сектора");

    VBox->addWidget(tabWidget);

    this->setLayout(VBox);

    resize(1400,800);
}

QStringList LoadStateCheck::makeTextBeautiful(QStringList txt)
{
    for(int i=0;i<txt.size();i++)
            if(subString(txt[i],"    TCH") || subString(txt[i],"   SDCCH")  || subString(txt[i],"   CBCH"))
            {
                txt[i] = "     "+txt[i];

            }
    return txt;
}

