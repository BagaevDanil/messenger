#include "tusersocket.h"

TUserSocket::TUserSocket(QObject *parent)
    : QTcpSocket(parent){}

void TUserSocket::SetUserLogin(const QString& login)
{
    UserLogin = login;
}
