#include "authorizationwindow.h"
#include "ui_authorizationwindow.h"

enum ETypeAction {
    AUTHORIZATION,
    MESSAGE,
};

TAuthorizationWindow::TAuthorizationWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TAuthorizationWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    socket->connectToHost("127.0.0.1", 2323);
    // if (error conn) repeat
}

TAuthorizationWindow::~TAuthorizationWindow()
{
    delete ui;
}

void TAuthorizationWindow::SendToServer(QString str)
{
    _Data.clear();
    QDataStream output(&_Data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_2);

    QString login = ui->lineEditLogin->text();
    QString pass = ui->lineEditPassword->text();
    output << ETypeAction::AUTHORIZATION;
    output << login;
    output << pass;

    socket->write(_Data);
}

void TAuthorizationWindow::SlotReadyRead()
{
    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_2);

    if (input.status() == QDataStream::Ok) {
        int typeAction;
        input >> typeAction;

        if (typeAction == ETypeAction::AUTHORIZATION) {
            QString login;
            input >> login;
            if (login == "") {
                // Неправильный логин или пароль
                return;
            }
            qDebug() << "Good:" << login;
            MainWindow* chatWindow = new MainWindow(login);
            chatWindow->show();
            this->close();
        }
    }
    else {
        // Error read
    }
}

void TAuthorizationWindow::on_pushButtonAuthorization_clicked()
{
    SendToServer("");
}

