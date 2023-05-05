#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "../common/common.h"
#include "textmessage.h"
#include <QScrollBar>

bool TChatWindow::HostExists()
{
    if (Connected) {
        return true;
    }

    Connected = ConnectToHost();
    while (!Connected) {
        qDebug() << "-Try connect to HOST";
        auto answer = QMessageBox::question(this, "Ошибка соедениния!", "Повторить попытку?");
        if (answer == QMessageBox::StandardButton::No) {
            qDebug() << "   Cancel connect to HOST";
            return false;
        }
        Connected = ConnectToHost();
    }
    qDebug() << "   Successfully connected";
    return false;
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
        // _Data.clear();
        //QDataStream output(&_Data, QIODevice::WriteOnly);
        //output.setVersion(QDataStream::Qt_6_2);

        //output << ETypeAction::CHECK_CONNECTION;
        //_Socket->write(_Data);

        if (_Socket->waitForReadyRead()) {

            //QByteArray bytes = _Socket->readAll();
            //if (bytes.contains("")) { // 200 OK
                return true;
            //}
        }
    }
    return false;
}

TChatWindow::TChatWindow(QString userLogin, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TChatWindow)
    , _Socket(nullptr)
    , _UserLogin(userLogin)
    , _Button(nullptr)
{
    ui->setupUi(this);

    ui->scrollArea->setWidgetResizable(true);

    _Container = new QWidget();
    ui->scrollArea->setWidget(_Container);
    _Layout = new QVBoxLayout(_Container);

    Connected = ConnectToHost();
}

void TChatWindow::GetPackMessageEarly()
{
    if (_Button) {
        delete _Button;
        _Button = nullptr;
    }
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << ETypeAction::MESSAGE_EARLY;
    output << _CurInd;

    _Socket->write(_Data);
}

void TChatWindow::SlotSockDisc()
{
    Connected = false;
    qDebug() << "-Disconnect HOST!";
}

TChatWindow::~TChatWindow()
{
    delete ui;
}

void TChatWindow::SendMsgToServer()
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    TMessageData msg(_UserLogin, ui->lineEdit->text(), "0");
    output << ETypeAction::MESSAGE;
    output << msg;

    _Socket->write(_Data);
}

void TChatWindow::SlotReadyRead()
{
    qDebug() << "-Ans for HOST :";
    QDataStream input(_Socket);
    input.setVersion(QDataStream::Qt_6_2);

    if (input.status() != QDataStream::Ok) {
        qDebug() << "   Error read!";
        return;
    }

    int typeAction;
    input >> typeAction;
    if (typeAction == ETypeAction::MESSAGE) {
        TMessageData msg;
        input >> msg;
        qDebug() << "   Message <" << msg.Login << "> : " << msg.Text << " | " << msg.Time;

        auto* msgUi = new TTextMessage(msg, this);
        _Layout->insertWidget(0, msgUi);
    }
    else if (typeAction == ETypeAction::MESSAGE_EARLY) {
        TMessagePack msgPack;
        input >> msgPack;
        _CurInd = msgPack.CurInd;
        qDebug() << "   Pack history : " << msgPack.SizePack << "(size) | " << msgPack.CurInd<< "(ind)";

        for (int i = 0; i < msgPack.SizePack; i++) {
            auto* msgUi = new TTextMessage(msgPack.ArrMessage[i], this);
            _Layout->addWidget(msgUi);
        }

        if (_CurInd != 0) {
            _Button = new QPushButton("Раннее", this);
            connect(_Button, SIGNAL(clicked()), this, SLOT(GetPackMessageEarly()));
            _Layout->addWidget(_Button);
        }
    }
    else if (typeAction == ETypeAction::CHECK_CONNECTION) {
        QDataStream input(_Socket);
        input.setVersion(QDataStream::Qt_6_2);
        QString s;
        input >> s;
        qDebug() << "   Check connection : " << s;
    }
}

void TChatWindow::on_pushButtonSend_clicked()
{
    if (HostExists()) {
        SendMsgToServer();
    }
}
