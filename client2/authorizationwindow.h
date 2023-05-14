#ifndef AUTHORIZATIONWINDOW_H
#define AUTHORIZATIONWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include "chatwindow.h"
#include "../common/common.h"

namespace Ui {
class TAuthorizationWindow;
}

class TAuthorizationWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum ETypePadge {
        AUTHORIZATION_PADGE,
        REGISTRATION_PADGE,
    };

public:
    explicit TAuthorizationWindow(QWidget *parent = nullptr);
    ~TAuthorizationWindow();
    bool HostExists();
    bool ConnectToHost();

private:
    Ui::TAuthorizationWindow *ui;
    QTcpSocket* _Socket;
    QByteArray _Data;
    bool Connected;

private:
    void AuthorizationServer();
    void RegistrationServer();

    template<class TypeData>
    void SendDataToServer(TypeData data, ETypeAction action);

public slots:
    void SlotReadyRead();
    void SlotSockDisc();

private slots:
    void on_pushButtonAuthorization_clicked();
    void on_pushButtonAddNewUser_clicked();
    void on_pushButtonRegistrationPadge_clicked();
    void on_pushButtonAuthorizationPadge_clicked();
};

#endif // AUTHORIZATIONWINDOW_H
