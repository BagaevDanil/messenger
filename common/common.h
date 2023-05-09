#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QVector>

enum ETypeAction {
    AUTHORIZATION,
    REGISTRATION,
    MESSAGE,
    CHECK_CONNECTION,
    MESSAGE_HISTORY,
    DOWNLOAD_FROM_CLIENT,
    DOWNLOAD_FROM_SERVER,
    SUBSCRIBE_TO_MESSAGES,
};

enum ETypeAnsRegistration {
    OK,
    LOGIN_BUSY,
    UNKNOWN_ERROR,
};

namespace HOST {
    const QString ADDRES = "127.0.0.1";
    const int PORT = 1234;
};

struct TUserInfo {
    QString Login;
    QString Password;

public:
    TUserInfo();
    TUserInfo(QString login, QString password);

    friend QDataStream& operator>> (QDataStream& in, TUserInfo& msg);
    friend QDataStream& operator<< (QDataStream& out, TUserInfo& msg);
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


struct TMessageData {
    enum ETypeMessage {
        TEXT,
        FILE,
    };

    TMessageData();
    TMessageData(QString login, QString text, QString time, ETypeMessage type);

    QString Login;
    QString Text;
    QString Time;
    ETypeMessage Type;
    int FileId;

    friend QDataStream& operator>> (QDataStream& in, TMessageData& msg);
    friend QDataStream& operator<< (QDataStream &out, TMessageData &msg);
};

struct TMessagePack {
    QVector<TMessageData> ArrMessage;
    int CurInd;
    int SizePack;

    friend QDataStream& operator>> (QDataStream& in, TMessagePack& msg);
    friend QDataStream& operator<< (QDataStream &out, TMessagePack &msg);
};

#endif // COMMON_H
