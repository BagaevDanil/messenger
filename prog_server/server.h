#ifndef TSERVER_H
#define TSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TServer : public QTcpServer
{
    Q_OBJECT
public:
    TServer();
    ~TServer();
    QTcpSocket* socket;

private:
     QSet<QTcpSocket*> _ArrSocket;
     QByteArray _Data;
     void SendToClient(QString str);

public slots:
    void startServer();
    void incomingConnection(qintptr socketDescriptor);
    void SlotReadyRead();
    void SlotSocketDisc();

};

#endif // TSERVER_H
