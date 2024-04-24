#include "settings.h"
#include "ui_settings.h"

extern QString recipientIpAdress;
extern QString name;
extern int recPort;
extern QString serverIP;

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    ui->lineEdit->setText(recipientIpAdress);
    ui->lineEdit_3->setText(name);
    ui->lineEdit_port->setText(QString::number(recPort));
    ui->lineEdit_serverIP->setText(serverIP);
}

//Settings::Settings(const QString &ipAdress, int port, const QString &name) :
//    ui(new Ui::Settings)
//{
//    ui->setupUi(this);
//    ui->lineEdit->setText(recipientIpAdress);
//    ui->lineEdit_2->setText(QString::number(recipientPort));
//    ui->lineEdit_3->setText(name);
//}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_pushButton_ok_clicked()
{
    recipientIpAdress = ui->lineEdit->text();
    name = ui->lineEdit_3->text();
    recPort = ui->lineEdit_port->text().toInt();
    serverIP = ui->lineEdit_serverIP->text();
    window()->close();
}


void Settings::on_pushButton_cancel_clicked()
{
    window()->close();
}

