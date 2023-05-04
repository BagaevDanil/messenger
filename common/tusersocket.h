#ifndef TUSERSOCKET_H
#define TUSERSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

class TUserSocket : public QTcpSocket
{
public:
    TUserSocket(QObject *parent = nullptr);
    void SetUserLogin(const QString& login);
    QString UserLogin;
};

#endif // TUSERSOCKET_H
