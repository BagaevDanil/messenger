#include "authorizationwindow.h"
#include "ui_authorizationwindow.h"
#include <QMessageBox>

enum ETypePadge {
    AUTHORIZATION_PADGE,
    REGISTRATION_PADGE,
};

bool TAuthorizationWindow::HostExists()
{
    if (Connected) {
        return true;
    }

    Connected = ConnectToHost();
    while (!Connected) {
        qDebug() << "-Try connect to HOST";
        auto answer = QMessageBox::question(nullptr, "Ошибка соедениния!", "Повторить попытку?");
        if (answer == QMessageBox::StandardButton::No) {
            qDebug() << "   Cancel connect to HOST";
            return false;
        }
        Connected = ConnectToHost();
    }
    qDebug() << "   Successfully connected";
    return true;
}

bool TAuthorizationWindow::ConnectToHost()
{
    if (_Socket) {
        delete _Socket;
    }
    _Socket = new QTcpSocket(this);
    connect(_Socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(_Socket, SIGNAL(disconnected()),this,SLOT(SlotSockDisc()));
    _Socket->connectToHost(HOST::ADDRES, HOST::PORT);

    if (_Socket->waitForConnected()) {
        if (_Socket->waitForReadyRead()) {
            return true;
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

template<class TypeData>
void TAuthorizationWindow::SendDataToServer(TypeData data, ETypeAction action)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    output << action;
    output << data;

    _Socket->write(_Data);
}

void TAuthorizationWindow::AuthorizationServer()
{
    QString login = ui->lineEditLogin->text();
    QString pass = ui->lineEditPassword->text();

    if (login.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Введите логин");
        return;
    }
    if (pass.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Введите пароль");
        return;
    }

    TUserInfo user(login, pass);
    SendDataToServer(user, ETypeAction::AUTHORIZATION);
}

void TAuthorizationWindow::RegistrationServer()
{

    QString login = ui->lineEditNewLogin->text();

    QString pass = ui->lineEditNewPassword->text();
    QString passRepeat = ui->lineEditNewPasswordRepeat->text();

    if (login.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Введите логин");
        return;
    }
    if (pass.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Введите пароль");
        return;
    }
    if (pass != passRepeat) {
        QMessageBox::critical(this, "Ошибка", "Пароли не совпадают");
        return;
    }

    TUserInfo user(login, pass);
    SendDataToServer(user, ETypeAction::REGISTRATION);
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
        qDebug() << "   Authorization try";

        if (login == "") {
            qDebug() << "   Uncorrected pass or login";
            QMessageBox::critical(this, "Ошибка входа", "Неверный логин или пароль");
            return;
        }
        qDebug() << "   Successful log in";

        TChatWindow* chatWindow = new TChatWindow(login);
        chatWindow->show();

        _Socket->disconnect();
        this->close();
    }
    else if (typeAction == ETypeAction::CHECK_CONNECTION) {
        QString ans;
        input >> ans;
        qDebug() << "Check connection : " << ans;
    }
    else if (typeAction == ETypeAction::REGISTRATION) {
        ETypeAnsRegistration ans;
        input >> ans;
        if (ans == ETypeAnsRegistration::OK) {
            QMessageBox::information(this, "Регистрация", "Новый пользователь успешно добавлен");
        }
        else if (ans == ETypeAnsRegistration::LOGIN_BUSY) {
            QMessageBox::information(this, "Регистрация", "Логин занят");
        }
        else if (ans == ETypeAnsRegistration::OK) {
            QMessageBox::information(this, "Регистрация", "Неизвестная ошибка на сервере\nПовторите попытку позже");
        }
    }
    else {
        qDebug() << "Error action";
        QByteArray buf = _Socket->readAll();
    }
}

void TAuthorizationWindow::on_pushButtonAuthorization_clicked()
{
    if (HostExists()) {
        AuthorizationServer();
    }
}


void TAuthorizationWindow::on_pushButtonAddNewUser_clicked()
{
    if (HostExists()) {
        RegistrationServer();
    }
}

void TAuthorizationWindow::on_pushButtonRegistrationPadge_clicked()
{
    ui->stackedWidget->setCurrentIndex(ETypePadge::REGISTRATION_PADGE);
}


void TAuthorizationWindow::on_pushButtonAuthorizationPadge_clicked()
{
    ui->stackedWidget->setCurrentIndex(ETypePadge::AUTHORIZATION_PADGE);
}

