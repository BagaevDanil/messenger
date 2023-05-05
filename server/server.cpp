#include "server.h"

TServer::TServer(){}

TServer::~TServer(){}

bool TServer::StartServer()
{
    if (this->listen(QHostAddress::Any, 2323))
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
}

void TServer::SendToClient(QString str, ETypeAction typeAction)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << typeAction;
    output << str;

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
        QString msg;
        input >> msg;
        qDebug() << "MSG : " << msg;

        SendToClient(msg, ETypeAction::MESSAGE);
    }
    else if (typeAction == ETypeAction::CHECK_CONNECTION) {
        QString msg = "200 OK";
        SendToClient(msg, ETypeAction::CHECK_CONNECTION);
    }
}

void TServer::SlotSocketDisc()
{
    qDebug() << "Disconnect";
    socket = (QTcpSocket*)sender();
    _ArrSocket.erase(_ArrSocket.find(socket));
    socket->deleteLater();
}
