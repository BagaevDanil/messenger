#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSqlDatabase>
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
    struct DataDownloadFileUser {
        bool Downloading;
        int  FileByteSize;
        QByteArray DataDownload;
        QString FileNameDownload;
        QString UserLogin;

        DataDownloadFileUser()
            : Downloading(false){}
    };

    struct TFile{
        QByteArray Data;
        QString Name;
    };

private:
     QSet<QTcpSocket*> _ArrSocket;
     QByteArray _Data;
     QVector<TMessageData> _ArrMessage;
     QVector<TFile> _ArrFile;
     QSqlDatabase _DB;

     //bool _Downloading;
     //int  _FileByteSize;
     //QByteArray _DataDownload;
     //QString _FileNameDownload;

     QMap<int, DataDownloadFileUser> mapDownloadData;

     template <class TypeData>
     void SendDataToAllClients(TypeData msg, ETypeAction typeAction);

     template <class TypeData>
     void SendDataToClient(TypeData data, ETypeAction typeAction, QTcpSocket* socket);

     void DownloaIterations(DataDownloadFileUser& userDownloadInfo, QTcpSocket* socket);

     void SendPackToClient(TMessagePack msgPack);
     void SendMsgToClient(TMessageData msg, ETypeAction typeAction = MESSAGE);
     bool UserVerification(QString login, QString pass);
     void SendFileToClient(QTcpSocket* socket, int fileId);
     ETypeAnsRegistration UserRegistration(QString login, QString pass);
     bool CheckingLoginAvailability(QString login);



public slots:  
    void incomingConnection(qintptr socketDescriptor);
    void SlotReadyRead();
    void SlotSocketDisc();

};

#endif // SERVER_H
