#include "mainwindow.h"
#include "ui_mainwindow.h"

enum ETypeAction {
    AUTHORIZATION,
    MESSAGE,
};

MainWindow::MainWindow(QString userLogin, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _Socket = new QTcpSocket(this);
    _UserLogin = userLogin;

    connect(_Socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(_Socket, SIGNAL(disconnected()), _Socket, SLOT(deleteLater()));
    _Socket->connectToHost("127.0.0.1", 2323);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SendToServer(QString str)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);
    output << ETypeAction::MESSAGE;
    output << str;
    _Socket->write(_Data);
}

void MainWindow::SlotReadyRead()
{
    QDataStream input(_Socket);
    input.setVersion(QDataStream::Qt_6_2);

    if (input.status() == QDataStream::Ok) {
        int i;
        input >> i;
        QString s;
        input >> s;
        qDebug() << i << s;
        ui->textBrowser->append(s);
    }
    else {
        // Ошибка при получении данных
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
    SendToServer(ui->lineEdit->text());
}

