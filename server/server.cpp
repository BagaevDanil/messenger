#include "server.h"
#include <QTime>
#include <QTimer>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>


int SIZE_PACK = 10;
int TIME_PAUSE_BEFORE_DOWNLOAD = 300;
int BYTE_DOWNLOAD_PACK_SIZE = 32000;

TServer::TServer(){}

TServer::~TServer(){}

bool TServer::StartServer()
{
    _DB = QSqlDatabase::addDatabase("QSQLITE");
    _DB.setDatabaseName("../messenger.db");
    if (!_DB.open()) {
        qDebug() << "-Error DB";
        return false;
    }
    qDebug() << "-Connect to DB";
    QSqlQuery* query = new QSqlQuery(_DB);
    //query->exec("DELETE FROM Messages");
    //query->exec("DELETE FROM Files");
    // bool result = query->exec("CREATE TABLE Messages (Login TEXT, Text TEXT, Time TEXT, Ind TEXT);");
    //QSqlQuery* query = new QSqlQuery(_DB);
    //bool result = query->exec("CREATE TABLE Files (Ind TEXT, Data BLOB, FileName TEXT);");
    //bool result = query->exec("ALTER TABLE Messages ADD IndFile TEXT;");
    /*if (result) {
        qDebug() << "True";
    }
    else {
        qDebug() << query->lastError().text();
    }*/

    LoadMsgFromDB();
    LoadFileFromDB();

    if (!this->listen(QHostAddress::Any, HOST::PORT)) {
        qDebug() << "-Error start server!";
        return false;
    }
    qDebug() << "-Start server!";

    return true;
}

void TServer::SaveFileToDB(TFile file)
{
    qDebug() << "   -Save file to DB:";
    QSqlQuery* query = new QSqlQuery(_DB);
    query->prepare(
        "INSERT INTO Files\
            (Ind, Data, FileName)\
         VALUES\
            (:ind, :data, :name)"
    );
    query->bindValue(":ind", file.Ind);
    query->bindValue(":data", file.Data);
    query->bindValue(":name", file.Name);
    bool result = query->exec();

    if (result) {
        qDebug() << "       Add :" << file.Ind << file.Data << file.Name;
    }
    else {
        qDebug() << "       Error :" << query->lastError().text();
    }
}

void TServer::LoadFileFromDB()
{
    qDebug() << "-Load files :";
    QSqlQuery* query = new QSqlQuery(_DB);
    query->exec(
        "SELECT\
                Ind, Data, FileName\
         FROM\
                Files"
    );
    _CurIndFiles = 0;
    while(query->next()) {
        int ind = query->value(0).toString().toInt();
        QByteArray data = query->value(1).toByteArray();
        QString fileName = query->value(2).toString();
        qDebug() << "   file:" << ind << fileName;

        if (ind != _CurIndFiles) {
            throw std::logic_error("Error matching indexes in the DB");
        }
        TFile file{data, fileName, _CurIndFiles++};
        _ArrFile.push_back(file);
    }
}

void TServer::SaveMsgToDB(TMessageData msg)
{
    qDebug() << "   -Save msg to DB:";
    QSqlQuery* query = new QSqlQuery(_DB);
    query->prepare(
        "INSERT INTO Messages\
            (Login, Text, Time, Ind, Type, IndFile)\
        VALUES\
            (:login, :text, :time, :index, :type, :indFile)"
    );
    query->bindValue(":login", msg.Login);
    query->bindValue(":text", msg.Text);
    query->bindValue(":time", msg.Time);
    query->bindValue(":index", QString::number(msg.Ind));
    query->bindValue(":type", QString::number(int(msg.Type)));
    query->bindValue(":indFile", QString::number(int(msg.FileId)));
    bool result = query->exec();

    if (result) {
        qDebug() << "       Add :" << msg.Ind << msg.Login << msg.Text << msg.Time << msg.Type << msg.FileId;
    }
    else {
        qDebug() << "       Error :" << query->lastError().text();
    }
}

void TServer::LoadMsgFromDB()
{
    qDebug() << "Load msg :";
    QSqlQuery* query = new QSqlQuery(_DB);
    query->exec(
        "SELECT\
            Login, Text, Time, Ind, Type, IndFile\
         FROM\
            Messages"
    );
    _CurInd = 0;

    while(query->next()) {
        QString login = query->value(0).toString();
        QString text = query->value(1).toString();
        QString time = query->value(2).toString();
        int ind = query->value(3).toString().toInt();
        int type = query->value(4).toString().toInt();
        int fileId = query->value(5).toString().toInt();
        qDebug() << "   msg:" << _CurInd << login << text << time << type << fileId;

        TMessageData msg(login, text, time, TMessageData::ETypeMessage(type), _CurInd++);
        msg.FileId = fileId;
        _ArrMessage.push_back(msg);
    }
}

void TServer::incomingConnection(qintptr socketDescriptor)
{
    _Socket = new QTcpSocket;
    _Socket->setSocketDescriptor(socketDescriptor);

    connect(_Socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(_Socket, SIGNAL(disconnected()), this, SLOT(SlotSocketDisc()));

    qDebug() << "-New client connected : " << socketDescriptor;
    // _ArrSocket.insert(socket);
    _MapDownloadData.insert(socketDescriptor, DataDownloadFileUser());

    SendDataToClient(QString("200ok"), ETypeAction::CHECK_CONNECTION, _Socket);
}

template <class TypeData>
void TServer::SendDataToAllClients(TypeData data, ETypeAction typeAction)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << typeAction;
    output << data;

    for (auto& sock : _ArrSocket) {
        sock->write(_Data);
    }
}

template <class TypeData>
void TServer::SendDataToClient(TypeData data, ETypeAction typeAction, QTcpSocket* socket)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << typeAction;
    output << data;

    socket->write(_Data);
}

bool TServer::UserVerification(QString login, QString pass)
{
    QSqlQuery* query = new QSqlQuery(_DB);
    query->prepare("SELECT * FROM Users WHERE Login = :login AND Password = :pass");
    query->bindValue(":login", login);
    query->bindValue(":pass", pass);
    query->exec();

    if (query->next()) {
        qDebug() << "   Пользователь найден : " << query->value(0);
        return true;
    }
    qDebug() << "   Пользователь не найден";
    return false;
}

bool TServer::CheckingLoginAvailability(QString login)
{
    QSqlQuery* query = new QSqlQuery(_DB);
    query->prepare("SELECT * FROM Users WHERE Login = :login");
    query->bindValue(":login", login);
    query->exec();

    if (query->next()) {
        return false;
    }
    return true;
}

ETypeAnsRegistration TServer::UserRegistration(QString login, QString pass)
{
    if (!CheckingLoginAvailability(login)) {
        return ETypeAnsRegistration::LOGIN_BUSY;
    }

    QSqlQuery* query = new QSqlQuery(_DB);
    query->prepare("INSERT INTO Users (Login, Password) VALUES (:login, :pass)");
    query->bindValue(":login", login);
    query->bindValue(":pass", pass);
    bool result = query->exec();

    if (result) {
        qDebug() << "Строка успешно добавлена";
        return ETypeAnsRegistration::OK;
    }

    qDebug() << "Ошибка: " << query->lastError().text();
    return ETypeAnsRegistration::UNKNOWN_ERROR;
}

void TServer::DownloaIterations(DataDownloadFileUser& userDownloadInfo, QTcpSocket* socket)
{
    QByteArray buf = socket->readAll();
    userDownloadInfo.DataDownload.push_back(buf);
    userDownloadInfo.FileByteSize -= buf.size();

    qDebug() << "   File:" << buf.size() << " | " << userDownloadInfo.FileByteSize;
    if (userDownloadInfo.FileByteSize <= 0) {
        userDownloadInfo.Downloading = false;
        qDebug() << "Finish downloading, size : " << userDownloadInfo.DataDownload.size();

        TFile file{userDownloadInfo.DataDownload, userDownloadInfo.FileNameDownload, _CurIndFiles++};
        _ArrFile.push_back(file);
        SaveFileToDB(file);

        TMessageData msg;
        msg.Time = QTime::currentTime().toString("hh:mm:ss");
        msg.Login = userDownloadInfo.UserLogin;
        msg.Text = userDownloadInfo.FileNameDownload;
        msg.Type = TMessageData::ETypeMessage::FILE;
        msg.FileId = file.Ind;
        msg.Ind = _CurInd++;
        _ArrMessage.push_back(msg);
        SaveMsgToDB(msg);

        SendDataToAllClients(msg, ETypeAction::MESSAGE);
    }
}

void TServer::SlotReadyRead()
{
    _Socket = (QTcpSocket*)sender();
    int userDescr = _Socket->socketDescriptor();
    auto& userDownloadInfo = _MapDownloadData[userDescr];

    qDebug() << "-Ans for CLIENT <" << userDescr << "> :";
    QDataStream input(_Socket);
    input.setVersion(QDataStream::Qt_6_2);

    if (input.status() != QDataStream::Ok) {
        qDebug() << "   Error read!";
        return;
    }

    if (userDownloadInfo.Downloading) {
        qDebug() << "   Downloading...";
        DownloaIterations(userDownloadInfo, _Socket);
        return;
    }

    int typeAction;
    input >> typeAction;

    if (typeAction == ETypeAction::AUTHORIZATION) {
        TUserInfo user;
        input >> user;
        qDebug() << "   Log in : " << user.Login << " | " << user.Password;

        if (!UserVerification(user.Login, user.Password)) {
            SendDataToClient(QString(""), ETypeAction::AUTHORIZATION, _Socket);
            return;
        }
        SendDataToClient(user.Login, ETypeAction::AUTHORIZATION, _Socket);
    }
    else if (typeAction == ETypeAction::MESSAGE) {
        TMessageData msg;
        input >> msg;
        msg.Time = QTime::currentTime().toString("hh:mm:ss");
        msg.Type = TMessageData::ETypeMessage::TEXT;
        msg.Ind = _CurInd++;
        _ArrMessage.push_back(msg);
        SaveMsgToDB(msg);

        qDebug() << "   Msg (" << msg.Login << ") : " << msg.Text << " | " << msg.Time;
        SendDataToAllClients(msg, ETypeAction::MESSAGE);
    }
    else if (typeAction == ETypeAction::CHECK_CONNECTION) {
        qDebug() << "   Check connection : " << "200ok";
        //QString msg = "200ok";
        //SendToClient(msg, ETypeAction::CHECK_CONNECTION);
    }
    else if (typeAction == ETypeAction::MESSAGE_HISTORY) {
        int ind;
        input >> ind;

        if (ind < 0) {
            ind = _ArrMessage.size();
        }

        TMessagePack msgPack;
        msgPack.SizePack = std::min(ind, SIZE_PACK);
        msgPack.CurInd = ind - msgPack.SizePack;
        for (int i = ind - 1; i >= ind - msgPack.SizePack; --i) {
            msgPack.ArrMessage.push_back(_ArrMessage[i]);
        }

        qDebug() << "   Pack history : " << msgPack.SizePack << "(size) | " << msgPack.CurInd << "(ind)";
        SendDataToClient(msgPack, ETypeAction::MESSAGE_HISTORY, _Socket);
    }
    else if (typeAction == ETypeAction::DOWNLOAD_FROM_CLIENT) {
        userDownloadInfo.Downloading = true;
        TDownloadFileIndo info;
        input >> info;

        userDownloadInfo.UserLogin = info.Login;
        userDownloadInfo.FileNameDownload = info.FileName;
        userDownloadInfo.FileByteSize = info.FileSize;
        userDownloadInfo.DataDownload.clear();
        qDebug() << "   Start download : " << userDownloadInfo.FileByteSize;
    }
    else if (typeAction == ETypeAction::DOWNLOAD_FROM_SERVER) {
        int fileId;
        input >> fileId;
        qDebug() << "   Send file to clietn : " << fileId;
        SendFileToClient(_Socket, fileId);
    }
    else if (typeAction == ETypeAction::REGISTRATION) {
        TUserInfo user;
        input >> user;
        qDebug() << "   New user : " << user.Login << " | " << user.Password;

        auto ansReg = UserRegistration(user.Login, user.Password);
        if (ansReg != ETypeAnsRegistration::OK) {
            qDebug() << "   Error add new user";
        }
        SendDataToClient(ansReg, ETypeAction::REGISTRATION, _Socket);
    }
    else if (typeAction == ETypeAction::SUBSCRIBE_TO_MESSAGES) {
        QString ans;
        input >> ans;
        qDebug() << "  Subscribe to message : " << _Socket->socketDescriptor();
        _ArrSocket.insert(_Socket);
    }
}

void TServer::SendFileToClient(QTcpSocket* socket, int fileId)
{
    qDebug() << "-Send file:";
    auto& byteArray = _ArrFile[fileId].Data;
    qDebug() << "   File: " << byteArray.size();

    SendDataToClient(int(byteArray.size()), ETypeAction::DOWNLOAD_FROM_SERVER, socket);

    QTimer::singleShot(TIME_PAUSE_BEFORE_DOWNLOAD, this, [this, byteArray, socket](){
        int readyByte;
        for (readyByte = 0; readyByte < byteArray.size(); readyByte += BYTE_DOWNLOAD_PACK_SIZE) {
            socket->write(byteArray.mid(readyByte, BYTE_DOWNLOAD_PACK_SIZE));
        }
        socket->write(byteArray.mid(readyByte, BYTE_DOWNLOAD_PACK_SIZE));
    });
    qDebug() << "   File sended";
}

void TServer::SlotSocketDisc()
{
    qDebug() << "-Disconnect CLIENT";
    _Socket = (QTcpSocket*)sender();
    auto itSocket = _ArrSocket.find(_Socket);
    if (itSocket != _ArrSocket.end()) {
        _ArrSocket.erase(itSocket);
    }
    _Socket->deleteLater();
}
