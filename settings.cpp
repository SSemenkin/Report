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

    IP = settings->value("BSC IP","10.181.66.10").toString();
    Login = settings->value("Login","Smena").toString();
    Password = settings->value("Password","Ericsson2@").toString();

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

QString Settings::getSettigns(QString key) const
{
    return settings->value(key,"").toString();
}

void Settings::saveAuthData(QString key, QString value) const
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

    settings->setValue("BSC IP",ui->IP->text());
    settings->setValue("Login",ui->Login->text());
    settings->setValue("Password",ui->password->text());

    QMessageBox::information(this,"Information","All data successfull recieved");
}

void Settings::loadSettings()
{
    ui->IP->setText(settings->value("BSC IP","10.181.66.10").toString());
    ui->Login->setText(settings->value("Login","Smena").toString());
    ui->password->setText(settings->value("Password","Ericsson2@").toString());
}
