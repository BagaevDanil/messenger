#include "myserver.h"

myserver::myserver(){}

myserver::~myserver(){}

void myserver::startServer()
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

void myserver::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "NEW CLIENT";

    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(SlotSocketDisc()));

    qDebug() << socketDescriptor << " : new client connected";

    // socket->write("You are connect");
    _ArrSocket.insert(socket);
}

void myserver::SendToClient(QString str, ETypeAction typeAction)
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

bool myserver::UserVerification(QString login, QString pass)
{
    if (login == "Danya" && pass == "qwe123") {
        return true;
    }
    return false;
}

void myserver::SlotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_2);

    if (input.status() == QDataStream::Ok) {
        int typeAction;
        input >> typeAction;

        if (typeAction == ETypeAction::AUTHORIZATION) {
            QString login;
            QString pass;
            input >> login >> pass;
            qDebug() << "Log in : " << login << " -- " << pass;

            if (UserVerification(login, pass)) {
                SendToClient(login, ETypeAction::AUTHORIZATION);
            }
            else {
                SendToClient("", ETypeAction::AUTHORIZATION);
            }
        }
        else if (typeAction == ETypeAction::MESSAGE) {
            QString msg;
            input >> msg;
            qDebug() << "MSG : " << msg;
            SendToClient(msg, ETypeAction::MESSAGE);
        }
    }
    else {
        qDebug() << "Error read!";
    }
}

void myserver::SlotSocketDisc()
{
    qDebug() << "Disconnect";
    socket = (QTcpSocket*)sender();
    _ArrSocket.erase(_ArrSocket.find(socket));
    socket->deleteLater();
}
