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

private:
     QSet<QTcpSocket*> _ArrSocket;
     QByteArray _Data;
     QVector<TMessageData> _ArrMessage;

     template <class T>
     void SendToClient(T msg, ETypeAction typeAction = MESSAGE);

     void SendPackToClient(TMessagePack msgPack);
     void SendMsgToClient(TMessageData msg, ETypeAction typeAction = MESSAGE);
     bool UserVerification(QString login, QString pass);


public slots:  
    void incomingConnection(qintptr socketDescriptor);
    void SlotReadyRead();
    void SlotSocketDisc();

};

#endif // SERVER_H
