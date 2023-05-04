#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>
#include <QTcpSocket>

enum ETypeAction {
    AUTHORIZATION,
    MESSAGE,
};

class myserver: public QTcpServer
{
    Q_OBJECT
public:
    myserver();
    ~myserver();

    QTcpSocket* socket;
    QByteArray Data;

private:
     QSet<QTcpSocket*> _ArrSocket;
     QByteArray _Data;
     void SendToClient(QString str, ETypeAction typeAction = MESSAGE);
     bool UserVerification(QString login, QString pass);


public slots:
    void startServer();
    void incomingConnection(qintptr socketDescriptor);
    void SlotReadyRead();
    void SlotSocketDisc();

};

#endif // MYSERVER_H
