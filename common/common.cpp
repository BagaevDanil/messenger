#include "common.h"

TUserInfo::TUserInfo(){}

TUserInfo::TUserInfo(QString login, QString password)
    : Login(login)
    , Password(password){}

QDataStream& operator>> (QDataStream& in, TUserInfo& info)
{
    in >> info.Login;
    in >> info.Password;
    return in;
}

QDataStream& operator<< (QDataStream& out, TUserInfo& info)
{
    out << info.Login;
    out << info.Password;
    return out;
}



TDownloadFileIndo::TDownloadFileIndo(){}

TDownloadFileIndo::TDownloadFileIndo(QString login, QString fileName, int fileSize)
    : Login(login)
    , FileName(fileName)
    , FileSize(fileSize){}

QDataStream& operator>> (QDataStream& in, TDownloadFileIndo& info)
{
    in >> info.Login;
    in >> info.FileName;

    QString str;
    in >> str;
    info.FileSize = str.toInt();

    return in;
}

QDataStream& operator<< (QDataStream& out, TDownloadFileIndo& info)
{
    out << info.Login;
    out << info.FileName;
    out << QString::number(info.FileSize);
    return out;
}



QDataStream& operator>> (QDataStream &in, TMessageData &msg)
{
    in >> msg.Login;
    in >> msg.Text;
    in >> msg.Time;

    QString s;

    in >> s;
    msg.Type = TMessageData::ETypeMessage(s.toInt());
    in >> s;
    msg.FileId = s.toInt();

    return in;
}

QDataStream& operator<< (QDataStream &out, TMessageData &msg)
{
    out << msg.Login;
    out << msg.Text;
    out << msg.Time;
    out << QString::number(msg.Type);
    out << QString::number(msg.FileId);
    return out;
}

TMessageData::TMessageData(){}

TMessageData::TMessageData(QString login, QString text, QString time, ETypeMessage type)
    : Login(login)
    , Text(text)
    , Time(time)
    , Type(type){}


QDataStream& operator>> (QDataStream &in, TMessagePack &msgPack)
{
    QString s;
    in >> s;
    msgPack.CurInd = s.toInt();
    in >> s;
    msgPack.SizePack = s.toInt();

    for (int i = 0; i < msgPack.SizePack; i++) {
        TMessageData msg;
        in >> msg;
        msgPack.ArrMessage.push_back(msg);
    }
    return in;
}

QDataStream& operator<< (QDataStream &out, TMessagePack &msgPack)
{
    out << QString::number(msgPack.CurInd);
    out << QString::number(msgPack.SizePack);

    for (int i = 0; i < msgPack.SizePack; i++) {
        out << msgPack.ArrMessage[i];
    }
    return out;
}
