#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "formtextmessage.h"
#include <QScrollBar>
#include <QTimer>
#include <QFile>
#include <QFileDialog>

const int TChatWindow::HEIGHT_AUTOMATIC_SCROLL_DOWN = 80;
const int TChatWindow::TIME_AUTOMATIC_SCROLL_DOWN = 50;
const int TChatWindow::TIME_AUTOMATIC_SCROLL_HISTORY = 80;
const int TChatWindow::TIME_PAUSE_BEFORE_DOWNLOAD = 300;
const int TChatWindow::BYTE_DOWNLOAD_PACK_SIZE = 32768;
const int TChatWindow::TIME_PAUSE_REQUEST_HISTORY = 300;

bool TChatWindow::HostExists()
{
    if (Connected) {
        return true;
    }

    Connected = ConnectToHost();
    while (!Connected) {
        qDebug() << "-Try connect to HOST";
        auto answer = QMessageBox::question(nullptr, "Ошибка соедениния!", "Повторить попытку?");
        if (answer == QMessageBox::StandardButton::No) {
            qDebug() << "   Cancel connect to HOST";
            return false;
        }
        Connected = ConnectToHost();
    }
    QTimer::singleShot(TIME_PAUSE_REQUEST_HISTORY, this, [this](){
        QMessageBox::information(this, "Подключение", "Подключение было восстановлено");
    });
    qDebug() << "   Successfully connected";
    return false;
}

bool TChatWindow::ConnectToHost()
{
    if (_Socket) {
        delete _Socket;
    }
    if (_SocketDownload) {
        delete _SocketDownload;
    }

    _Socket = new QTcpSocket(this);
    connect(_Socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(_Socket, SIGNAL(disconnected()),this,SLOT(SlotSockDisc()));
    _Socket->connectToHost(HOST::ADDRES, HOST::PORT);

    _SocketDownload = new QTcpSocket(this);
    connect(_SocketDownload, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(_SocketDownload, SIGNAL(disconnected()),this,SLOT(SlotSockDisc()));
    _SocketDownload->connectToHost(HOST::ADDRES, HOST::PORT);

    if (_Socket->waitForConnected()) {
        if (_Socket->waitForReadyRead()) {
            while (_Layout->count() > 1) {
                auto* item = _Layout->takeAt(1);
                auto* widget = item->widget();
                if (widget) {
                    widget->deleteLater();
                }
            }
            SendDataToServer(QString(""), ETypeAction::SUBSCRIBE_TO_MESSAGES);
            QTimer::singleShot(TIME_PAUSE_REQUEST_HISTORY, this, [this](){
                SendDataToServer(-1, ETypeAction::MESSAGE_HISTORY);
            });
            return true;
        }
    }
    return false;
}

TChatWindow::TChatWindow(QString userLogin, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TChatWindow)
    , _Socket(nullptr)
    , _SocketDownload(nullptr)
    , Connected(false)
    , _CurInd(-1)
    , _editingNow(false)
    , _UserLogin(userLogin)
    , _Button(nullptr)
    , _Downloading(false)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    ui->pushButtonEditingCancel->setVisible(false);
    ui->pushButtonSendApply->setVisible(false);

    _TextField = new TCustomTextEdit(this);
    _TextField->setGeometry(ui->lineEditFake->geometry());
    ui->lineEditFake->setVisible(false);
    _TextField->setStyleSheet(ui->lineEditFake->styleSheet());

    connect(_TextField, SIGNAL(sendMsg()), this, SLOT(TextFieldPress()));
    connect(ui->pushButtonSend, SIGNAL(clicked()), this, SLOT(TextFieldPress()));

    ui->scrollArea->setWidgetResizable(true);
    ui->pushButtonToBottom->setVisible(false);

    _Container = ui->scrollAreaWidgetContents;
    _Layout = ui->verticalLayout;

    connect(ui->scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(ChangeVericalScroll(int)));

    Connected = ConnectToHost();
    if (!Connected) {
        throw std::logic_error("No connection");
    }
}

void TChatWindow::ChangeVericalScroll(int value)
{
    if (ui->scrollArea->verticalScrollBar()->maximum() - value > HEIGHT_AUTOMATIC_SCROLL_DOWN) {
        ui->pushButtonToBottom->setVisible(true);
    }
    else {
        ui->pushButtonToBottom->setVisible(false);
    }
    emit ChengeFieldView(value, ui->scrollArea->height());
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
    while (_Layout->count()) {
        auto* widget = _Layout->takeAt(0)->widget();
        if (widget) {
            widget->deleteLater();
        }
    }
    _TextField->deleteLater();
    _Socket->deleteLater();
    _SocketDownload->deleteLater();
    _Button->deleteLater();
    delete ui;
}

template<class TypeData>
void TChatWindow::SendDataToServer(TypeData data, ETypeAction action,  QTcpSocket* socket)
{
    if (!socket) {
        socket = _Socket;
    }

    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << action;
    output << data;

    socket->write(_Data);
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
        SendDataToServer(info, ETypeAction::DOWNLOAD_FROM_CLIENT, _SocketDownload);
    }
    else {
        qDebug() << "   Error file open";
        return;
    }

    StartLocalLoad(byteArray.size());
    QTimer::singleShot(TIME_PAUSE_BEFORE_DOWNLOAD, this, [this, byteArray](){
        int byteSend;
        for (byteSend = 0; byteSend < byteArray.size(); byteSend += BYTE_DOWNLOAD_PACK_SIZE) {
            UpdateLocalLoad(BYTE_DOWNLOAD_PACK_SIZE);
            _SocketDownload->write(byteArray.mid(byteSend, BYTE_DOWNLOAD_PACK_SIZE));
        }
        _SocketDownload->write(byteArray.mid(byteSend, byteArray.size() - byteSend));
        FinishLocalLoad();
    });
}

void TChatWindow::FinishLocalLoad()
{
    int TIME_PAUSE_FINISH_DOWNLOAD = 600;
    ui->progressBar->setValue(ui->progressBar->maximum());
    QTimer::singleShot(TIME_PAUSE_FINISH_DOWNLOAD, [this] {
        ui->progressBar->setVisible(false);
        _TextField->setVisible(true);
    });
}

void TChatWindow::UpdateLocalLoad(int size)
{
    ui->progressBar->setValue(ui->progressBar->value() + size);
}

void TChatWindow::StartLocalLoad(int size)
{
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(size);
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(true);
    _TextField->setVisible(false);
}

void TChatWindow::DownloadFileFromHost(TFormFileMessage* file)
{
    if (_Downloading) {
        QMessageBox::critical(this, "Ошибка", "Перед новым скачиванием дождитесь окончания загрузок");
        return;
    }

    qDebug() << "Download File From Host : " << file->GetFileID();
    _FileNameDownload = QFileDialog::getSaveFileName(
                        this,
                        "Open file",
                        "../" + file->GetFileName(),
                        "All files (*.*)"
                    );
    if (!_FileNameDownload.isEmpty()) {
        _FormFile = file;
        SendDataToServer(file->GetFileID(), ETypeAction::DOWNLOAD_FROM_SERVER, _SocketDownload);
    }
}

void TChatWindow::AddNewMessage(TMessageData msg, bool toBottom)
{
    TFormMessage* msgForm;
    bool isMyMsg = msg.Login == _UserLogin;
    if (msg.Type == TMessageData::ETypeMessage::TEXT) {
        qDebug() << "   MessageText <" << msg.Ind << msg.Login << "> : " << msg.Text << " | " << msg.Time;
        msgForm = new TFormTextMessage(msg, isMyMsg, msg.IsEditing, this);
        connect(msgForm, SIGNAL(EditingMsg(int, QString)), this, SLOT(SlotEditingMsg(int, QString)));
    }
    else {
        qDebug() << "   MessageFile <" << msg.Ind << msg.Login << "> : " << msg.Text << " | " << msg.Time;
        msgForm = new TFormFileMessage(msg, isMyMsg, this);
        connect(msgForm, SIGNAL(DownloadFile(TFormFileMessage*)), this, SLOT(DownloadFileFromHost(TFormFileMessage*)));
    }

    if (toBottom) {
        _Layout->addWidget(msgForm);
    }
    else {
        _Layout->insertWidget(1, msgForm);
    }
    _MapMsg[msg.Ind] = msgForm;
    connect(this, SIGNAL(ChengeFieldView(int, int)), msgForm, SLOT(CheckFieldOfView(int, int)));
    connect(msgForm, SIGNAL(ChangeViewStatus(int)), this, SLOT(ChangeViewStatusSlot(int)));
}

void TChatWindow::ChangeViewStatusSlot(int id)
{
    qDebug() << "-Change View Status Slot :" << id;
    SendDataToServer(id, ETypeAction::VIEWED_MESSAGE);
}

void TChatWindow::DownloaIterations()
{
    qDebug() << "   Downloading..";
    QByteArray buf = _SocketDownload->readAll();
    _DataDownload.push_back(buf);
    _FileByteSize -= buf.size();
    _FormFile->UpdateDownload(buf.size());

    qDebug() << "   File:" << buf.size() << " | " << _FileByteSize;
    if (_FileByteSize <= 0) {
        _Downloading = false;
        _FormFile->FinishDownload();
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
    auto* socket = (QTcpSocket*)sender();

    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_2);

    if (input.status() != QDataStream::Ok) {
        qDebug() << "   Error read!";
        return;
    }

    if (_Downloading && socket == _SocketDownload) {
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
            _Button->setStyleSheet(ui->pushButtonToBottom->styleSheet());
            connect(_Button, SIGNAL(clicked()), this, SLOT(GetHistoryPack()));
            _Layout->insertWidget(1, _Button);
        }
    }
    else if (typeAction == ETypeAction::CHECK_CONNECTION) {
        QByteArray buf = socket->readAll();
        QString ans(buf);
        qDebug() << "   Check connection : " << ans;
    }
    else if (typeAction == ETypeAction::DOWNLOAD_FROM_SERVER) {
        _Downloading = true;
        input >> _FileByteSize;
        _DataDownload.clear();
        _FormFile->StartDownload(_FileByteSize);
        qDebug() << "   Start download: " << _FileByteSize;
    }
    else if (typeAction == ETypeAction::EDIT_MESSAGE) {
        TEditMessageInfo msg;
        input >> msg;
        qDebug() << "   Edit message :" << msg.MsgId;
        _MapMsg[msg.MsgId]->SetText(msg.NewText);
        _MapMsg[msg.MsgId]->SetEditMark(true);
    }
    else if (typeAction == ETypeAction::VIEWED_MESSAGE) {
        int id;
        input >> id;
        qDebug() << "   Viewed message : " << id;
        _MapMsg[id]->SetView();
    }
    else {
        qDebug() << "  Error action";
        QByteArray buf = _Socket->readAll();
    }
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

void TChatWindow::SlotEditingMsg(int msgId, QString text)
{
    qDebug() << "-Editing Msg :" << msgId;
    _CurEditMsgId = msgId;
    TurnOnEditMod(text);
}

void TChatWindow::TurnOnEditMod(QString text)
{
    _editingNow = true;
    _TextField->setPlainText(text);
    ui->pushButtonEditingCancel->setVisible(true);
    ui->pushButtonSendApply->setVisible(true);
    ui->pushButtonSend->setVisible(false);
    ui->pushButtonSendFIle->setVisible(false);
}

void TChatWindow::TurnOffEditMod()
{
    _editingNow = false;
    _TextField->clear();
    ui->pushButtonEditingCancel->setVisible(false);
    ui->pushButtonSendApply->setVisible(false);
    ui->pushButtonSend->setVisible(true);
    ui->pushButtonSendFIle->setVisible(true);
}

void TChatWindow::TextFieldPress()
{
    if (_editingNow) {
        return;
    }

    qDebug() << "-Press send";
    if (_TextField->toPlainText().isEmpty()) {
        return;
    }

    if (HostExists()) {
        TMessageData msg(_UserLogin, _TextField->toPlainText(), "", TMessageData::ETypeMessage::TEXT, false, false);
        _TextField->clear();
        SendDataToServer(msg, ETypeAction::MESSAGE);
    }
}

void TChatWindow::on_pushButtonSendFIle_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Open file", "../", "All files (*.*)");
    if (path.isEmpty()) {
        return;
    }
    SendFileToServer(path);
}


void TChatWindow::on_pushButtonEditingCancel_clicked()
{
    qDebug() << "   Cancel editing Msg";
    TurnOffEditMod();
}


void TChatWindow::on_pushButtonSendApply_clicked()
{
    qDebug() << "   Apply editing Msg";
    if (_TextField->toPlainText().isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Пустое сообщение!");
        return;
    }

    TEditMessageInfo msg(_TextField->toPlainText(), _CurEditMsgId);
    SendDataToServer(msg, ETypeAction::EDIT_MESSAGE);
    TurnOffEditMod();
}

