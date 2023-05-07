#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "textmessage.h"
#include "formfilemessage.h"
#include <QScrollBar>
#include <QTimer>
#include <QFile>
#include <QFileDialog>

int HEIGHT_AUTOMATIC_SCROLL_DOWN = 80;
int TIME_AUTOMATIC_SCROLL_DOWN = 50;
int TIME_AUTOMATIC_SCROLL_HISTORY = 100;
int TIME_PAUSE_BEFORE_DOWNLOAD = 300;
int BYTE_DOWNLOAD_PACK_SIZE = 32768;

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

    connect(ui->scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(ChangeVericalScroll(int)));

    Connected = ConnectToHost();
}

void TChatWindow::ChangeVericalScroll(int value)
{
    if (ui->scrollArea->verticalScrollBar()->maximum() - value > 100) {
        ui->pushButtonToBottom->setVisible(true);
    }
    else {
        ui->pushButtonToBottom->setVisible(false);
    }
}

void TChatWindow::GetHistoryPack()
{
    if (_Button) {
        delete _Button;
        _Button = nullptr;
    }
    SendDataToServer(_CurInd, ETypeAction::MESSAGE_HISTORY);
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

template<class TypeData>
void TChatWindow::SendDataToServer(TypeData data, ETypeAction action)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << action;
    output << data;

    _Socket->write(_Data);
}

void TChatWindow::SendFileToServer(QString fileName)
{
    qDebug() << "-Send file:";
    auto* sendFile = new QFile(fileName);
    QByteArray byteArray;

    if (sendFile->open(QFile::ReadOnly)) {
        qDebug() << "   File is open";

        byteArray = sendFile->readAll();
        QString fileName = QFileInfo(*sendFile).fileName();
        qDebug() << "   File: " << fileName << byteArray.size();

        TDownloadFileIndo info(_UserLogin, fileName, int(byteArray.size()));
        SendDataToServer(info, ETypeAction::DOWNLOAD_FROM_CLIENT);
    }
    else {
        qDebug() << "   Error file open";
        return;
    }

    QTimer::singleShot(TIME_PAUSE_BEFORE_DOWNLOAD, this, [this, byteArray](){
        int byteSend;
        for (byteSend = 0; byteSend < byteArray.size(); byteSend += BYTE_DOWNLOAD_PACK_SIZE) {
            _Socket->write(byteArray.mid(byteSend, BYTE_DOWNLOAD_PACK_SIZE));
        }
        _Socket->write(byteArray.mid(byteSend, byteArray.size() - byteSend));
    });
}

void TChatWindow::DownloadFileFromHost(int fileId, QString fileName)
{
    qDebug() << "Download File From Host : " << fileId;
    _FileNameDownload = QFileDialog::getSaveFileName(
                        this,
                        "Open file",
                        "../" + fileName,
                        "All files (*.*)"
                    );
    if (!_FileNameDownload.isEmpty()) {
        SendDataToServer(fileId, ETypeAction::DOWNLOAD_FROM_SERVER);
    }
}

void TChatWindow::AddNewMessage(TMessageData msg, bool toBottom)
{
    QWidget* msgForm;
    if (msg.Type == TMessageData::ETypeMessage::TEXT) {
        qDebug() << "   MessageText <" << msg.Login << "> : " << msg.Text << " | " << msg.Time;
        msgForm = new TTextMessage(msg, this);
    }
    else {
        qDebug() << "   MessageFile <" << msg.Login << "> : " << msg.Text << " | " << msg.Time;
        msgForm = new TFormFileMessage(msg, this);
        connect(msgForm, SIGNAL(DownloadFile(int, QString)), this, SLOT(DownloadFileFromHost(int, QString)));
    }

    if (toBottom) {
        _Layout->addWidget(msgForm);
    }
    else {
        _Layout->insertWidget(0, msgForm);
    }
}

void TChatWindow::DownloaIterations()
{
    qDebug() << "   Downloading..";
    QByteArray buf = _Socket->readAll();
    _DataDownload.push_back(buf);
    _FileByteSize -= buf.size();

    qDebug() << "   File:" << buf.size() << " | " << _FileByteSize;
    if (_FileByteSize <= 0) {
        _Downloading = false;
        qDebug() << "Finish downloading, size : " << _DataDownload.size();

        QFile file(_FileNameDownload);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(_DataDownload);
            file.close();
        }
    }
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
        DownloaIterations();
        return;
    }

    int typeAction;
    input >> typeAction;
    if (typeAction == ETypeAction::MESSAGE) {
        TMessageData msg;
        input >> msg;
        AddNewMessage(msg, true);

        int maxH = ui->scrollArea->verticalScrollBar()->maximum();
        int curH = ui->scrollArea->verticalScrollBar()->value();
        if (maxH - curH < HEIGHT_AUTOMATIC_SCROLL_DOWN) {
            QTimer::singleShot(TIME_AUTOMATIC_SCROLL_DOWN, this, &TChatWindow::on_pushButtonToBottom_clicked);
        }
    }
    else if (typeAction == ETypeAction::MESSAGE_HISTORY) {
        TMessagePack msgPack;
        input >> msgPack;
        qDebug() << "   Pack history : " << msgPack.SizePack << "(size) | " << msgPack.CurInd<< "(ind)";

        for (const auto& msg : msgPack.ArrMessage) {
            AddNewMessage(msg, false);
        }

        if (_CurInd < 0) {
            QTimer::singleShot(TIME_AUTOMATIC_SCROLL_HISTORY, this, &TChatWindow::on_pushButtonToBottom_clicked);
        }
        else {
            int oldH = ui->scrollArea->verticalScrollBar()->maximum();
            QTimer::singleShot(TIME_AUTOMATIC_SCROLL_HISTORY, this, [this, oldH](){
                SetShiftHistory(oldH);
            });
        }

        _CurInd = msgPack.CurInd;
        if (_CurInd > 0) {
            _Button = new QPushButton("Раннее", this);
            connect(_Button, SIGNAL(clicked()), this, SLOT(GetHistoryPack()));
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
        input >> _FileByteSize;
        _DataDownload.clear();
        qDebug() << "   Start download: " << _FileByteSize;
    }
}

void TChatWindow::on_pushButtonSend_clicked()
{
    on_lineEdit_returnPressed();
}

void TChatWindow::SetShiftHistory(int prevH)
{
    int newH = ui->scrollArea->verticalScrollBar()->maximum();
    ui->scrollArea->verticalScrollBar()->setValue(newH - prevH);
}

void TChatWindow::on_pushButtonToBottom_clicked()
{
    int maxH = ui->scrollArea->verticalScrollBar()->maximum();
    ui->scrollArea->verticalScrollBar()->setValue(maxH);
}


void TChatWindow::on_lineEdit_returnPressed()
{
    if (HostExists()) {
        TMessageData msg(_UserLogin, ui->lineEdit->text(), "", TMessageData::ETypeMessage::TEXT);
        SendDataToServer(msg, ETypeAction::MESSAGE);
    }
}


void TChatWindow::on_pushButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Open file", "../", "All files (*.*)");
    if (path.isEmpty()) {
        return;
    }
    SendFileToServer(path);
}

