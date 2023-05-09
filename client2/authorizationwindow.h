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
    explicit TAuthorizationWindow(QWidget *parent = nullptr);
    ~TAuthorizationWindow();
    bool HostExists();
    bool ConnectToHost();

    template<class TypeData>
    void SendDataToServer(TypeData data, ETypeAction action);



private:
    Ui::TAuthorizationWindow *ui;
    QTcpSocket* _Socket;
    QByteArray _Data;
    void AuthorizationServer();
    void RegistrationServer();
    bool Connected;

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
