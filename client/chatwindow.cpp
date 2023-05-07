#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "../common/common.h"
#include "textmessage.h"
#include "formfilemessage.h"
#include <QScrollBar>
#include <QTimer>
#include <QFile>
#include <QFileDialog>

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
        if (_Socket->waitForReadyRead()) {
            return true;
        }
    }
    return false;
}

TChatWindow::TChatWindow(QString userLogin, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TChatWindow)
    , _Socket(nullptr)
    , _CurInd(-1)
    , _UserLogin(userLogin)
    , _Button(nullptr)
    , _Downloading(false)
{
    ui->setupUi(this);

    ui->scrollArea->setWidgetResizable(true);

    _Container = new QWidget();
    ui->scrollArea->setWidget(_Container);
    _Layout = new QVBoxLayout(_Container);

    connect(ui->scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(MoveScroll(int)));

    Connected = ConnectToHost();
}

void TChatWindow::MoveScroll(int value)
{
    if (ui->scrollArea->verticalScrollBar()->maximum() - value > 100) {
        ui->pushButtonToBottom->setVisible(true);
    }
    else {
        ui->pushButtonToBottom->setVisible(false);
    }
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

    TMessageData msg(_UserLogin, ui->lineEdit->text(), "0", TMessageData::ETypeMessage::TEXT);
    output << ETypeAction::MESSAGE;
    output << msg;

    _Socket->write(_Data);
}

void TChatWindow::SendFileToServer(QString fileName)
{
    qDebug() << "-Send file:";
    auto* sendFile = new QFile(fileName);
    QByteArray byteArray;

    if (sendFile->open(QFile::ReadOnly)) {
        qDebug() << "   File is open";

        _Data.clear();
        QDataStream output(&_Data, QIODevice::WriteOnly);
        output.setVersion(QDataStream::Qt_6_2);

        byteArray = sendFile->readAll();
        QString fileName = QFileInfo(*sendFile).fileName();
        qDebug() << "   File: " << fileName << byteArray.size();

        output << ETypeAction::MESSAGE_FILE;
        output << _UserLogin;
        output << fileName;
        output << int(byteArray.size());
        _Socket->write(_Data);
    }
    else {
        qDebug() << "   Error file open";
        return;
    }

    QTimer::singleShot(300, this, [this, byteArray](){
        int BYTE_PACK_SIZE = 32000;
        int i = 0;
        for (i = 0; i < byteArray.size(); i += BYTE_PACK_SIZE) {
            _Socket->write(byteArray.mid(i, BYTE_PACK_SIZE));
        }
        _Socket->write(byteArray.mid(i, byteArray.size() - i));
    });
}

void TChatWindow::DownloadFileFromHost(int id, QString fileName)
{
    qDebug() << "Download File From Host : " << id;

    _FileNameDownload = QFileDialog::getSaveFileName(
                        this,
                        "Open file",
                        "../" + fileName,
                        "All files (*.*)"
                    );

    qDebug() << "    Save to : " << _FileNameDownload;
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << ETypeAction::DOWNLOAD_FROM_SERVER;
    output << id;

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

    if (_Downloading) {
        qDebug() << "   Downloading..";
        QByteArray buf = _Socket->readAll();
        _DataDownload.push_back(buf);
        _FileByteSize -= buf.size();

        qDebug() << "   File:" << buf.size() << " | " << _FileByteSize;
        if (_FileByteSize <= 0) {
            _Downloading = false;
            qDebug() << "Finish downloading, size : " << _DataDownload.size();

            QFile file;
            QString path = _FileNameDownload;
            file.setFileName(path);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(_DataDownload); // записываем данные в файл
                file.close();
            }
        }
        return;
    }

    int typeAction;
    input >> typeAction;
    if (typeAction == ETypeAction::MESSAGE) {
        TMessageData msg;
        input >> msg;

        if (msg.Type == TMessageData::ETypeMessage::TEXT) {
            qDebug() << "   MessageText <" << msg.Login << "> : " << msg.Text << " | " << msg.Time;
            auto* msgForm = new TTextMessage(msg, this);
            _Layout->addWidget(msgForm);
        }
        else {
            qDebug() << "   MessageFile <" << msg.Login << "> : " << msg.Text << " | " << msg.Time;
            auto*  msgForm = new TFormFileMessage(msg, this);
            _Layout->addWidget(msgForm);
            connect(msgForm, SIGNAL(DownloadFile(int, QString)), this, SLOT(DownloadFileFromHost(int, QString)));
        }

        if (ui->scrollArea->verticalScrollBar()->maximum() - ui->scrollArea->verticalScrollBar()->value() < 80) {
            QTimer::singleShot(50, this, &TChatWindow::on_pushButtonToBottom_clicked);
        }
    }
    else if (typeAction == ETypeAction::MESSAGE_EARLY) {
        TMessagePack msgPack;
        input >> msgPack;
        qDebug() << "   Pack history : " << msgPack.SizePack << "(size) | " << msgPack.CurInd<< "(ind)";

        for (int i = 0; i < msgPack.SizePack; i++) {
            //auto* msgUi = new TTextMessage(msgPack.ArrMessage[i], this);
            //_Layout->insertWidget(0, msgUi);
            auto& msg = msgPack.ArrMessage[i];
            if (msg.Type == TMessageData::ETypeMessage::TEXT) {
                qDebug() << "   MessageText <" << msg.Login << "> : " << msg.Text << " | " << msg.Time;
                auto* msgForm = new TTextMessage(msg, this);
                _Layout->insertWidget(0, msgForm);
            }
            else {
                qDebug() << "   MessageFile <" << msg.Login << "> : " << msg.Text << " | " << msg.Time;
                auto*  msgForm = new TFormFileMessage(msg, this);
                _Layout->insertWidget(0, msgForm);
                connect(msgForm, SIGNAL(DownloadFile(int, QString)), this, SLOT(DownloadFileFromHost(int, QString)));
            }
        }

        int oldH = ui->scrollArea->verticalScrollBar()->maximum();
        if (_CurInd == -1) {
            QTimer::singleShot(100, this, &TChatWindow::on_pushButtonToBottom_clicked);
        }
        else {
            QTimer::singleShot(100, this, [this, oldH](){
                SetShiftHistory(oldH);
            });
        }

        _CurInd = msgPack.CurInd;
        if (_CurInd != 0) {
            _Button = new QPushButton("Раннее", this);
            connect(_Button, SIGNAL(clicked()), this, SLOT(GetPackMessageEarly()));
            _Layout->insertWidget(0, _Button);
        }
    }
    else if (typeAction == ETypeAction::CHECK_CONNECTION) {
        QDataStream input(_Socket);
        input.setVersion(QDataStream::Qt_6_2);
        QString s;
        input >> s;
        qDebug() << "   Check connection : " << s;
    }
    else if (typeAction == ETypeAction::DOWNLOAD_FROM_SERVER) {
        _Downloading = true;
        QString fileName;
        input >> _FileByteSize;
        input >> fileName;
        _DataDownload.clear();

        qDebug() << "   Start download: " << _FileByteSize;
    }
}

void TChatWindow::on_pushButtonSend_clicked()
{
    if (HostExists()) {
        SendMsgToServer();
    }
}

void TChatWindow::SetShiftHistory(int h)
{
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum() - h);
    qDebug() << ui->scrollArea->verticalScrollBar()->maximum() << h;
}

void TChatWindow::on_pushButtonToBottom_clicked()
{
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
}


void TChatWindow::on_lineEdit_returnPressed()
{
    if (HostExists()) {
        SendMsgToServer();
    }
}


void TChatWindow::on_pushButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(
                        this,
                        "Open file",
                        "../",
                        "All files (*.*)"
                    );
    SendFileToServer(path);
}

