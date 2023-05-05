#include "common.h"

QDataStream& operator>> (QDataStream &in, TMessageData &msg)
{
    in >> msg.Login;
    in >> msg.Text;
    in >> msg.Time;
}

QDataStream& operator<< (QDataStream &out, TMessageData &msg)
{
    out << msg.Login;
    out << msg.Text;
    out << msg.Time;
}

TMessageData::TMessageData(){}

TMessageData::TMessageData(QString login, QString text, QString time)
    : Login(login)
    , Text(text)
    , Time(time){}


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
}

QDataStream& operator<< (QDataStream &out, TMessagePack &msgPack)
{
    out << QString::number(msgPack.CurInd);
    out << QString::number(msgPack.SizePack);

    for (int i = 0; i < msgPack.SizePack; i++) {
        out << msgPack.ArrMessage[i];
    }
}
