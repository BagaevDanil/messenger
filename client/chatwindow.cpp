#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "../common/common.h"

void TChatWindow::HostExists()
{
    if (!Connected) {
        Connected = ConnectToHost();
    }

    while (!Connected) {
        auto answer = QMessageBox::question(this, "Ошибка соедениния!", "Повторить попытку?");
        if (answer == QMessageBox::StandardButton::No) {
            return;
        }
        qDebug() << "Try connect...";
        Connected = ConnectToHost();
    }
    qDebug() << "Connected!";
}

bool TChatWindow::ConnectToHost()
{
    if (_Socket) {
        delete _Socket;
    }
    _Socket = new QTcpSocket(this);
    connect(_Socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(_Socket, SIGNAL(disconnected()),this,SLOT(SlotSockDisc()));
    _Socket->connectToHost(HOST::ADDRES, HOST::PORT);

    if (_Socket->waitForConnected()) {
        _Data.clear();
        QDataStream output(&_Data, QIODevice::WriteOnly);
        output.setVersion(QDataStream::Qt_6_2);

        output << ETypeAction::CHECK_CONNECTION;
        _Socket->write(_Data);

        if (_Socket->waitForReadyRead()) {
            QByteArray bytes = _Socket->readAll();
            if (bytes.contains("")) { // 200 OK
                return true;
            }
        }
    }
    return false;
}

TChatWindow::TChatWindow(QString userLogin, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TChatWindow)
    , _Socket(nullptr)
    , _UserLogin(userLogin)
{
    ui->setupUi(this);
    Connected = ConnectToHost();
}


void TChatWindow::SlotSockDisc()
{
    Connected = false;
    qDebug() << "Disconnect host!";
}

TChatWindow::~TChatWindow()
{
    delete ui;
}

void TChatWindow::SendToServer(QString str)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);
    output << ETypeAction::MESSAGE;
    output << str;
    _Socket->write(_Data);
}

void TChatWindow::SlotReadyRead()
{
    QDataStream input(_Socket);
    input.setVersion(QDataStream::Qt_6_2);

    if (input.status() != QDataStream::Ok) {
        qDebug() << "Error read!";
        return;
    }

    int type;
    input >> type;
    if (type == ETypeAction::MESSAGE) {
        QString s;
        input >> s;
        qDebug() << type << s;
        ui->textBrowser->append(s);
    }
}

void TChatWindow::on_pushButtonSend_clicked()
{
    HostExists();
    SendToServer(ui->lineEdit->text());
}
