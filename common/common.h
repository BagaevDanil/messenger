#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QVector>

enum ETypeAction {
    AUTHORIZATION,
    MESSAGE,
    CHECK_CONNECTION,
    MESSAGE_HISTORY,
    DOWNLOAD_FROM_CLIENT,
    DOWNLOAD_FROM_SERVER,
    SUBSCRIBE_NEW_MESSAGE,
};

namespace HOST {
    const QString ADDRES = "127.0.0.1";
    const int PORT = 1234;
};


struct TDownloadFileIndo {
    QString Login;
    QString FileName;
    int FileSize;

public:
    TDownloadFileIndo();
    TDownloadFileIndo(QString login, QString fileName, int fileSize);

    friend QDataStream& operator>> (QDataStream& in, TDownloadFileIndo& msg);
    friend QDataStream& operator<< (QDataStream& out, TDownloadFileIndo& msg);
};


class TMessageData {
public:
    enum ETypeMessage {
        TEXT,
        FILE,
    };

public:
    TMessageData();
    TMessageData(QString login, QString text, QString time, ETypeMessage type);

public:
    QString Login;
    QString Text;
    QString Time;
    ETypeMessage Type;
    int FileId;

    friend QDataStream& operator>> (QDataStream& in, TMessageData& msg);
    friend QDataStream& operator<< (QDataStream &out, TMessageData &msg);
};

class TMessagePack {
public:
    QVector<TMessageData> ArrMessage;
    int CurInd;
    int SizePack;

    friend QDataStream& operator>> (QDataStream& in, TMessagePack& msg);
    friend QDataStream& operator<< (QDataStream &out, TMessagePack &msg);
};

#endif // COMMON_H
