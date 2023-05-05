#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include "../common/common.h"

class TServer: public QTcpServer
{
    Q_OBJECT

public:
    TServer();
    ~TServer();
    bool StartServer();

public:
    QTcpSocket* socket;
    QByteArray Data;

private:
     QSet<QTcpSocket*> _ArrSocket;
     QByteArray _Data;
     void SendToClient(QString str, ETypeAction typeAction = MESSAGE);
     bool UserVerification(QString login, QString pass);


public slots:  
    void incomingConnection(qintptr socketDescriptor);
    void SlotReadyRead();
    void SlotSocketDisc();

};

#endif // SERVER_H
