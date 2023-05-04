#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonConnect_clicked()
{
    qDebug() << "connectToHost";
    // socket->SetUserLogin("Danya");
    socket->connectToHost("127.0.0.1", 2323);
}

void MainWindow::SendToServer(QString str)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);
    output << str;
    socket->write(_Data);
}

void MainWindow::SlotReadyRead()
{
    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_2);
    if (input.status() == QDataStream::Ok) {
        QString s;
        input >> s;
        ui->textBrowser->append(s);
    }
    else {
        ui->textBrowser->append("Error read!");
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
    SendToServer(ui->lineEdit->text());
}

