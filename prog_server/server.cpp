#include "server.h"

TServer::TServer(){}

TServer::~TServer(){}

void TServer::startServer()
{
    if (this->listen(QHostAddress::Any, 2323))
    {
        qDebug() << "Start server!";
    }
    else
    {
        qDebug() << "Error!";
    }
}

void TServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "NEW CLIENT";

    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(SlotSocketDisc()));

    qDebug() << socketDescriptor << " : new client connected";

    socket->write("You are connect");
    _ArrSocket.insert(socket);
}

void TServer::SendToClient(QString str)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);
    output << str;
    for (auto& sock : _ArrSocket) {
        sock->write(_Data);
    }
}

void TServer::SlotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_2);
    if (input.status() == QDataStream::Ok) {
        QString s;
        input >> s;
        qDebug() << "Read : " << s;
        SendToClient(s);
    }
    else {
        qDebug() << "Error read!";
    }
}

void TServer::SlotSocketDisc()
{
    qDebug() << "Disconnect";
    socket = (QTcpSocket*)sender();
    _ArrSocket.erase(_ArrSocket.find(socket));
    socket->deleteLater();
}
