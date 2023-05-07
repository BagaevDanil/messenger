#include "server.h"
#include <QTime>
#include <QTimer>
#include <QFile>

int SIZE_PACK = 10;

TServer::TServer(){}

TServer::~TServer(){}

bool TServer::StartServer()
{
    if (this->listen(QHostAddress::Any, HOST::PORT))
    {
        qDebug() << "-Start server!";
        return true;
    }
    qDebug() << "-Error start server!";
    return false;
}

void TServer::incomingConnection(qintptr socketDescriptor)
{

    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(SlotSocketDisc()));

    qDebug() << "-New client connected : " << socketDescriptor;
    _ArrSocket.insert(socket);
    mapDownloadData.insert(socketDescriptor, DataDownloadFileUser());

    //
    TMessagePack msgPack;
    if (_ArrMessage.empty()) {
        msgPack.SizePack = 0;
        msgPack.CurInd = 0;
        SendPackToClient(msgPack);
        return;
    }

    msgPack.SizePack = std::min(int(_ArrMessage.size()), SIZE_PACK);
    msgPack.CurInd = _ArrMessage.size() - msgPack.SizePack;

    for (int i = _ArrMessage.size() - 1; i >= _ArrMessage.size() - msgPack.SizePack; --i) {
        msgPack.ArrMessage.push_back(_ArrMessage[i]);
    }
    SendPackToClient(msgPack);
}

template <class T>
void TServer::SendToClient(T msg, ETypeAction typeAction)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << typeAction;
    output << msg;

    for (auto& sock : _ArrSocket) {
        sock->write(_Data);
    }
}

void TServer::SendPackToClient(TMessagePack msgPack)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << ETypeAction::MESSAGE_HISTORY;
    output << msgPack;

    for (auto& sock : _ArrSocket) {
        sock->write(_Data);
    }
}

void TServer::SendMsgToClient(TMessageData msg, ETypeAction typeAction)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << typeAction;
    output << msg;

    for (auto& sock : _ArrSocket) {
        sock->write(_Data);
    }
}

bool TServer::UserVerification(QString login, QString pass)
{
    if (login == "Danya" && pass == "qwe123") {
        return true;
    }
    return false;
}

void TServer::SlotReadyRead()
{
    socket = (QTcpSocket*)sender();
    int userDescr = socket->socketDescriptor();
    qDebug() << "-Ans for CLIENT <" << userDescr << "> :";
    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_2);

    if (input.status() != QDataStream::Ok) {
        qDebug() << "   Error read!";
        return;
    }

    if (mapDownloadData[userDescr]._Downloading) {
        qDebug() << "   Downloading..";
        QByteArray buf = socket->readAll();
        mapDownloadData[userDescr]._DataDownload.push_back(buf);
        mapDownloadData[userDescr]._FileByteSize -= buf.size();

        qDebug() << "   File:" << buf.size() << " | " << mapDownloadData[userDescr]._FileByteSize;
        if (mapDownloadData[userDescr]._FileByteSize <= 0) {
            mapDownloadData[userDescr]._Downloading = false;
            qDebug() << "Finish downloading, size : " << mapDownloadData[userDescr]._DataDownload.size();

            TMessageData msg;
            msg.Time = QTime::currentTime().toString("hh:mm:ss");
            msg.Login = mapDownloadData[userDescr].UserLogin;
            msg.Text = mapDownloadData[userDescr]._FileNameDownload;
            msg.Type = TMessageData::ETypeMessage::FILE;
            msg.FileId = _ArrFile.size();
            _ArrMessage.push_back(msg);
            _ArrFile.push_back({mapDownloadData[userDescr]._DataDownload, mapDownloadData[userDescr]._FileNameDownload});
            SendMsgToClient(msg, ETypeAction::MESSAGE);
        }
        return;
    }

    int typeAction;
    input >> typeAction;

    if (typeAction == ETypeAction::AUTHORIZATION) {
        QString login;
        QString pass;
        input >> login >> pass;
        qDebug() << "   Log in : " << login << " | " << pass;

        if (!UserVerification(login, pass)) {
            SendToClient("", ETypeAction::AUTHORIZATION);
            return;
        }
        SendToClient(login, ETypeAction::AUTHORIZATION);
    }
    else if (typeAction == ETypeAction::MESSAGE) {
        TMessageData msg;
        input >> msg;
        msg.Time = QTime::currentTime().toString("hh:mm:ss");
        msg.Type = TMessageData::ETypeMessage::TEXT;
        _ArrMessage.push_back(msg);

        qDebug() << "   Msg (" << msg.Login << ") : " << msg.Text << " | " << msg.Time;
        SendMsgToClient(msg, ETypeAction::MESSAGE);
    }
    else if (typeAction == ETypeAction::CHECK_CONNECTION) {
        qDebug() << "   Check connection : " << "200ok";
        //QString msg = "200ok";
        //SendToClient(msg, ETypeAction::CHECK_CONNECTION);
    }
    else if (typeAction == ETypeAction::MESSAGE_HISTORY) {
        int ind;
        input >> ind;

        TMessagePack msgPack;
        msgPack.SizePack = std::min(ind, SIZE_PACK);
        msgPack.CurInd = ind - msgPack.SizePack;

        qDebug() << "   Pack history : " << msgPack.SizePack << "(size) | " << msgPack.CurInd << "(ind)";

        for (int i = ind - 1; i >= ind - msgPack.SizePack; --i) {
            msgPack.ArrMessage.push_back(_ArrMessage[i]);
        }
        SendToClient(msgPack, ETypeAction::MESSAGE_HISTORY);
    }
    else if (typeAction == ETypeAction::DOWNLOAD_FROM_CLIENT) {
        mapDownloadData[userDescr]._Downloading = true;
        TDownloadFileIndo info;
        input >> info;

        mapDownloadData[userDescr].UserLogin = info.Login;
        mapDownloadData[userDescr]._FileNameDownload = info.FileName;
        mapDownloadData[userDescr]._FileByteSize = info.FileSize;
        mapDownloadData[userDescr]._DataDownload.clear();
        qDebug() << "   Start download : " << mapDownloadData[userDescr]._FileByteSize;
    }
    else if (typeAction == ETypeAction::DOWNLOAD_FROM_SERVER) {
        int fileId;
        input >> fileId;
        qDebug() << "   Send file to clietn : " << fileId;
        SendFileToClient(socket, fileId);
    }
}

void TServer::SendFileToClient(QTcpSocket* socket, int fileId)
{
    qDebug() << "-Send file:";

    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);
    auto& byteArray = _ArrFile[fileId].Data;
    qDebug() << "   File: " << byteArray.size();

    output << ETypeAction::DOWNLOAD_FROM_SERVER;
    output << int(byteArray.size());

    socket->write(_Data);

    //
    QTimer::singleShot(300, this, [this, byteArray, socket](){
        int BYTE_PACK_SIZE = 32000;
        int i = 0;
        for (i = 0; i < byteArray.size(); i += BYTE_PACK_SIZE) {
            socket->write(byteArray.mid(i, BYTE_PACK_SIZE));
        }
        socket->write(byteArray.mid(i, BYTE_PACK_SIZE));
    });
    qDebug() << "   File sended";
}

void TServer::SlotSocketDisc()
{
    socket = (QTcpSocket*)sender();
    qDebug() << "-Disconnect CLIENT : " << socket->socketDescriptor();
    _ArrSocket.erase(_ArrSocket.find(socket));
    socket->deleteLater();
}
