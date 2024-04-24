#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "server.h"
#include "cqueue.h"
#include "message.h"

#include <thread>
#include <WinSock2.h>
#include <QMessageBox>

net::Server server(net::Server::getLocalIP(), 8080);
ConcurrentQueue<Message> msgQueue;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_ip->setText(QString::fromStdString(server.ip()));
    server.Start();
    std::thread recieving(&net::Server::startRecieving, server);
    recieving.detach();
    std::thread updatingChat(&MainWindow::updateChat, this);
    updatingChat.detach();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString recipientIpAdress{};
QString name{};
int recPort{};
QString serverIP = QString::fromStdString(net::Server::getLocalIP());

void MainWindow::on_pushButton_settings_clicked()
{
    Settings settings;
    settings.setModal(true);
    settings.exec();
    server.Start();
}

void MainWindow::on_pushButton_send_clicked()
{
    if (recipientIpAdress.isNull() || !recPort) {
        QMessageBox::critical(this, "Error", "Check the entered IP address or port");
        return;
    }
    if (ui->lineEdit_message->text().isNull()) {
        return;
    }
    Message msg(ui->lineEdit_message->text().toStdString().c_str(), std::chrono::system_clock::now(), name.toStdString().c_str());
    server.sendToAddress(recipientIpAdress.toStdString(), recPort, msg);
    ui->lineEdit_message->clear();
    //std::this_thread::sleep_for(std::chrono::microseconds(100));
}

void MainWindow::updateChat()
{
    while (true) {
        if (!msgQueue.empty()) {
            auto msg = msgQueue.pop();
            //std::cout << "kal " << msg.content << " " << msg.name << "\n";
            std::time_t tm = std::chrono::system_clock::to_time_t(msg.time);
            auto time = localtime(&tm);
            ui->textBrowser_chat->append(QString::fromStdString(std::string(msg.name) + " [" + std::to_string(time->tm_hour) + ":" + std::to_string(time->tm_min) + ":" + std::to_string(time->tm_sec) + "]:\n" + std::string(msg.content)));
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

