#include "server.h"
#include <QTime>
#include <QTimer>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>


int SIZE_PACK = 10;
int TIME_PAUSE_BEFORE_DOWNLOAD = 300;
int BYTE_DOWNLOAD_PACK_SIZE = 32000;

TServer::TServer(){}

TServer::~TServer(){}

bool TServer::StartServer()
{
    _DB = QSqlDatabase::addDatabase("QSQLITE");
    _DB.setDatabaseName("../messenger.db");
    if (!_DB.open()) {
        qDebug() << "-Error DB";
        return false;
    }
    qDebug() << "-Connect to DB";

    if (!this->listen(QHostAddress::Any, HOST::PORT)) {
        qDebug() << "-Error start server!";
        return false;
    }
    qDebug() << "-Start server!";

    return true;
}

void TServer::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(SlotSocketDisc()));

    qDebug() << "-New client connected : " << socketDescriptor;
    // _ArrSocket.insert(socket);
    mapDownloadData.insert(socketDescriptor, DataDownloadFileUser());

    SendDataToClient(QString("200ok"), ETypeAction::CHECK_CONNECTION, socket);
}

template <class TypeData>
void TServer::SendDataToAllClients(TypeData data, ETypeAction typeAction)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << typeAction;
    output << data;

    for (auto& sock : _ArrSocket) {
        sock->write(_Data);
    }
}

template <class TypeData>
void TServer::SendDataToClient(TypeData data, ETypeAction typeAction, QTcpSocket* socket)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << typeAction;
    output << data;

    socket->write(_Data);
}

bool TServer::UserVerification(QString login, QString pass)
{
    QSqlQuery* query = new QSqlQuery(_DB);
    query->prepare("SELECT * FROM Users WHERE Login = :login AND Password = :pass");
    query->bindValue(":login", login);
    query->bindValue(":pass", pass);
    query->exec();

    if (query->next()) {
        qDebug() << "   Пользователь найден : " << query->value(0);
        return true;
    }
    qDebug() << "   Пользователь не найден";
    return false;
}

bool TServer::CheckingLoginAvailability(QString login)
{
    QSqlQuery* query = new QSqlQuery(_DB);
    query->prepare("SELECT * FROM Users WHERE Login = :login");
    query->bindValue(":login", login);
    query->exec();

    if (query->next()) {
        return false;
    }
    return true;
}

ETypeAnsRegistration TServer::UserRegistration(QString login, QString pass)
{
    if (!CheckingLoginAvailability(login)) {
        return ETypeAnsRegistration::LOGIN_BUSY;
    }

    QSqlQuery* query = new QSqlQuery(_DB);
    query->prepare("INSERT INTO Users (Login, Password) VALUES (:login, :pass)");
    query->bindValue(":login", login);
    query->bindValue(":pass", pass);
    bool result = query->exec();

    if (result) {
        qDebug() << "Строка успешно добавлена";
        return ETypeAnsRegistration::OK;
    }

    qDebug() << "Ошибка: " << query->lastError().text();
    return ETypeAnsRegistration::UNKNOWN_ERROR;
}

void TServer::DownloaIterations(DataDownloadFileUser& userDownloadInfo, QTcpSocket* socket)
{
    QByteArray buf = socket->readAll();
    userDownloadInfo.DataDownload.push_back(buf);
    userDownloadInfo.FileByteSize -= buf.size();

    qDebug() << "   File:" << buf.size() << " | " << userDownloadInfo.FileByteSize;
    if (userDownloadInfo.FileByteSize <= 0) {
        userDownloadInfo.Downloading = false;
        qDebug() << "Finish downloading, size : " << userDownloadInfo.DataDownload.size();

        TMessageData msg;
        msg.Time = QTime::currentTime().toString("hh:mm:ss");
        msg.Login = userDownloadInfo.UserLogin;
        msg.Text = userDownloadInfo.FileNameDownload;
        msg.Type = TMessageData::ETypeMessage::FILE;
        msg.FileId = _ArrFile.size();

        _ArrMessage.push_back(msg);
        _ArrFile.push_back({userDownloadInfo.DataDownload, userDownloadInfo.FileNameDownload});
        SendDataToAllClients(msg, ETypeAction::MESSAGE);
    }
}

void TServer::SlotReadyRead()
{
    socket = (QTcpSocket*)sender();
    int userDescr = socket->socketDescriptor();
    auto& userDownloadInfo = mapDownloadData[userDescr];

    qDebug() << "-Ans for CLIENT <" << userDescr << "> :";
    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_2);

    if (input.status() != QDataStream::Ok) {
        qDebug() << "   Error read!";
        return;
    }

    if (userDownloadInfo.Downloading) {
        qDebug() << "   Downloading...";
        DownloaIterations(userDownloadInfo, socket);
        return;
    }

    int typeAction;
    input >> typeAction;

    if (typeAction == ETypeAction::AUTHORIZATION) {
        TUserInfo user;
        input >> user;
        qDebug() << "   Log in : " << user.Login << " | " << user.Password;

        if (!UserVerification(user.Login, user.Password)) {
            SendDataToClient(QString(""), ETypeAction::AUTHORIZATION, socket);
            return;
        }
        SendDataToClient(user.Login, ETypeAction::AUTHORIZATION, socket);
    }
    else if (typeAction == ETypeAction::MESSAGE) {
        TMessageData msg;
        input >> msg;
        msg.Time = QTime::currentTime().toString("hh:mm:ss");
        msg.Type = TMessageData::ETypeMessage::TEXT;
        _ArrMessage.push_back(msg);

        qDebug() << "   Msg (" << msg.Login << ") : " << msg.Text << " | " << msg.Time;
        SendDataToAllClients(msg, ETypeAction::MESSAGE);
    }
    else if (typeAction == ETypeAction::CHECK_CONNECTION) {
        qDebug() << "   Check connection : " << "200ok";
        //QString msg = "200ok";
        //SendToClient(msg, ETypeAction::CHECK_CONNECTION);
    }
    else if (typeAction == ETypeAction::MESSAGE_HISTORY) {
        int ind;
        input >> ind;

        if (ind < 0) {
            ind = _ArrMessage.size();
        }

        TMessagePack msgPack;
        msgPack.SizePack = std::min(ind, SIZE_PACK);
        msgPack.CurInd = ind - msgPack.SizePack;
        for (int i = ind - 1; i >= ind - msgPack.SizePack; --i) {
            msgPack.ArrMessage.push_back(_ArrMessage[i]);
        }

        qDebug() << "   Pack history : " << msgPack.SizePack << "(size) | " << msgPack.CurInd << "(ind)";
        SendDataToClient(msgPack, ETypeAction::MESSAGE_HISTORY, socket);
    }
    else if (typeAction == ETypeAction::DOWNLOAD_FROM_CLIENT) {
        userDownloadInfo.Downloading = true;
        TDownloadFileIndo info;
        input >> info;

        userDownloadInfo.UserLogin = info.Login;
        userDownloadInfo.FileNameDownload = info.FileName;
        userDownloadInfo.FileByteSize = info.FileSize;
        userDownloadInfo.DataDownload.clear();
        qDebug() << "   Start download : " << userDownloadInfo.FileByteSize;
    }
    else if (typeAction == ETypeAction::DOWNLOAD_FROM_SERVER) {
        int fileId;
        input >> fileId;
        qDebug() << "   Send file to clietn : " << fileId;
        SendFileToClient(socket, fileId);
    }
    else if (typeAction == ETypeAction::REGISTRATION) {
        TUserInfo user;
        input >> user;
        qDebug() << "   New user : " << user.Login << " | " << user.Password;

        auto ansReg = UserRegistration(user.Login, user.Password);
        if (ansReg != ETypeAnsRegistration::OK) {
            qDebug() << "   Error add new user";
        }
        SendDataToClient(ansReg, ETypeAction::REGISTRATION, socket);
    }
    else if (typeAction == ETypeAction::SUBSCRIBE_TO_MESSAGES) {
        QString ans;
        input >> ans;
        qDebug() << "  Subscribe to message : " << socket->socketDescriptor();
        _ArrSocket.insert(socket);
    }
}

void TServer::SendFileToClient(QTcpSocket* socket, int fileId)
{
    qDebug() << "-Send file:";
    auto& byteArray = _ArrFile[fileId].Data;
    qDebug() << "   File: " << byteArray.size();

    SendDataToClient(int(byteArray.size()), ETypeAction::DOWNLOAD_FROM_SERVER, socket);

    QTimer::singleShot(TIME_PAUSE_BEFORE_DOWNLOAD, this, [this, byteArray, socket](){
        int readyByte;
        for (readyByte = 0; readyByte < byteArray.size(); readyByte += BYTE_DOWNLOAD_PACK_SIZE) {
            socket->write(byteArray.mid(readyByte, BYTE_DOWNLOAD_PACK_SIZE));
        }
        socket->write(byteArray.mid(readyByte, BYTE_DOWNLOAD_PACK_SIZE));
    });
    qDebug() << "   File sended";
}

void TServer::SlotSocketDisc()
{
    qDebug() << "-Disconnect CLIENT";
    socket = (QTcpSocket*)sender();
    auto itSocket = _ArrSocket.find(socket);
    if (itSocket != _ArrSocket.end()) {
        _ArrSocket.erase(itSocket);
    }
    socket->deleteLater();
}
