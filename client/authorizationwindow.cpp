#include "authorizationwindow.h"
#include "ui_authorizationwindow.h"

enum ETypeAction {
    AUTHORIZATION,
    MESSAGE,
    CHECK_CONNECTION,
};

void TAuthorizationWindow::HostExists()
{
    if (!Connected) {
        Connected = ConnectToHost();
    }

    while (!Connected) {
        auto answer = QMessageBox::question(this, "Ошибка соедениния!", "Повторить попытку?");
        if (answer == QMessageBox::StandardButton::No) {
            return;
        }
        qDebug() << "Try connect...";
        Connected = ConnectToHost();
    }
    qDebug() << "Connected!";
}

bool TAuthorizationWindow::ConnectToHost()
{
    if (_Socket) {
        delete _Socket;
    }
    _Socket = new QTcpSocket(this);
    connect(_Socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(_Socket, SIGNAL(disconnected()),this,SLOT(SlotSockDisc()));
    _Socket->connectToHost("127.0.0.1", 2323);

    if (_Socket->waitForConnected()) {
        _Data.clear();
        QDataStream output(&_Data, QIODevice::WriteOnly);
        output.setVersion(QDataStream::Qt_6_2);

        output << ETypeAction::CHECK_CONNECTION;
        _Socket->write(_Data);

        if (_Socket->waitForReadyRead()) {
            QByteArray bytes = _Socket->readAll();
            if (bytes.contains("")) { // 200 OK
                return true;
            }
        }
    }
    return false;
}

TAuthorizationWindow::TAuthorizationWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TAuthorizationWindow)
    , _Socket(nullptr)
    , Connected(false)
{
    ui->setupUi(this);
    Connected = ConnectToHost();
}

void TAuthorizationWindow::SlotSockDisc()
{
    Connected = false;
    qDebug() << "Disconnect host!";
}

TAuthorizationWindow::~TAuthorizationWindow()
{
    delete ui;
}

void TAuthorizationWindow::AuthorizationServer()
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    QString login = ui->lineEditLogin->text();
    QString pass = ui->lineEditPassword->text();
    output << ETypeAction::AUTHORIZATION;
    output << login;
    output << pass;

    _Socket->write(_Data);
}

void TAuthorizationWindow::SlotReadyRead()
{
    QDataStream input(_Socket);
    input.setVersion(QDataStream::Qt_6_2);

    if (input.status() != QDataStream::Ok) {
        qDebug() << "Error read";
        return;
    }
    int typeAction;
    input >> typeAction;

    if (typeAction == ETypeAction::AUTHORIZATION) {
        QString login;
        input >> login;
        if (login == "") {
            qDebug() << "Uncorrected pass or login";
            return;
        }

        qDebug() << "Authorization :" << login;
        TChatWindow* chatWindow = new TChatWindow(login);
        chatWindow->show();

        _Socket->disconnect();
        this->close();
    }
}

void TAuthorizationWindow::on_pushButtonAuthorization_clicked()
{
    HostExists();
    AuthorizationServer();
}

