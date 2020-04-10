#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setWindowTitle("Connection Settings");
    this->setWindowFlag(Qt::WindowContextHelpButtonHint,false);
    settings = new QSettings("Smena LTD.","Report",this);

    IP = settings->value("bsc/bscIP","").toString();
    Login = settings->value("bsc/bscLogin","").toString();
    Password = settings->value("bsc/bscPassword","").toString();

    loadSettings();

    connect(ui->buttonBox,&QDialogButtonBox::accepted,this,&Settings::saveSettings);
}

Settings::~Settings()
{
    delete settings;
    delete ui;
}

QString Settings::getPassword() const
{
    return Password;
}

QString Settings::getLogin() const
{
    return Login;
}

QString Settings::getIP() const
{
    return IP;
}

QVariant Settings::getSettigns(QString key) const
{
    return settings->value(key,"").toString();
}

void Settings::saveAuthData(QString key, QVariant value) const
{
    settings->setValue(key,value);
}

void Settings::saveSettings()
{
    if(ui->IP->text().isEmpty())
    {
        QMessageBox::warning(this,"Warning","IP BSC is empty");
        return;
    }
    if(ui->password->text().isEmpty())
    {
        QMessageBox::warning(this,"Warning","password is empty");
        return;
    }
    if(ui->Login->text().isEmpty())
    {
        QMessageBox::warning(this,"Warning","Login is empty");
        return;
    }

    settings->setValue("bsc/bscIP",ui->IP->text());
    settings->setValue("bsc/bscLogin",ui->Login->text());
    settings->setValue("bsc/bscPassword",ui->password->text());

    QMessageBox::information(this,"Information","All data successfull recieved");
}

void Settings::loadSettings()
{
    ui->IP->setText(settings->value("bsc/bscIP","").toString());
    ui->Login->setText(settings->value("bsc/bscLogin","").toString());
    ui->password->setText(settings->value("bsc/bscPassword","").toString());
}
