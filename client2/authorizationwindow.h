#ifndef AUTHORIZATIONWINDOW_H
#define AUTHORIZATIONWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include "chatwindow.h"

namespace Ui {
class TAuthorizationWindow;
}

class TAuthorizationWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TAuthorizationWindow(QWidget *parent = nullptr);
    ~TAuthorizationWindow();
    void HostExists();
    bool ConnectToHost();


private:
    Ui::TAuthorizationWindow *ui;
    QTcpSocket* _Socket;
    QByteArray _Data;
    void AuthorizationServer();
    bool Connected;

public slots:
    void SlotReadyRead();
    void SlotSockDisc();

private slots:
    void on_pushButtonAuthorization_clicked();
};

#endif // AUTHORIZATIONWINDOW_H
