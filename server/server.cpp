#include "server.h"
#include <QTime>

int SIZE_PACK = 10;

TServer::TServer(){}

TServer::~TServer(){}

bool TServer::StartServer()
{
    if (this->listen(QHostAddress::Any, HOST::PORT))
    {
        qDebug() << "Start server!";
        return true;
    }
    qDebug() << "Server error!";
    return false;
}

void TServer::incomingConnection(qintptr socketDescriptor)
{

    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(SlotSocketDisc()));

    qDebug() << "New client connected : " << socketDescriptor;
    _ArrSocket.insert(socket);

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

    output << ETypeAction::MESSAGE_EARLY;
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
    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_2);

    if (input.status() != QDataStream::Ok) {
        qDebug() << "Error read!";
        return;
    }

    int typeAction;
    input >> typeAction;

    if (typeAction == ETypeAction::AUTHORIZATION) {
        QString login;
        QString pass;
        input >> login >> pass;
        qDebug() << "Log in : " << login << " -- " << pass;

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
        _ArrMessage.push_back(msg);

        qDebug() << "Msg (" << msg.Login << ") : " << msg.Text << " | " << msg.Time;
        SendMsgToClient(msg, ETypeAction::MESSAGE);
    }
    else if (typeAction == ETypeAction::CHECK_CONNECTION) {
        //QString msg = "200ok";
        //SendToClient(msg, ETypeAction::CHECK_CONNECTION);
    }
    else if (typeAction == ETypeAction::MESSAGE_EARLY) {
        int ind;
        input >> ind;

        TMessagePack msgPack;
        msgPack.SizePack = std::min(ind, SIZE_PACK);
        msgPack.CurInd = ind - msgPack.SizePack;

        for (int i = ind - 1; i >= ind - msgPack.SizePack; --i) {
            msgPack.ArrMessage.push_back(_ArrMessage[i]);
        }
        SendToClient(msgPack, ETypeAction::MESSAGE_EARLY);
    }
}

void TServer::SlotSocketDisc()
{
    qDebug() << "Disconnect";
    socket = (QTcpSocket*)sender();
    _ArrSocket.erase(_ArrSocket.find(socket));
    socket->deleteLater();
}
