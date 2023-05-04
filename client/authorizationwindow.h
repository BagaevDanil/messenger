#ifndef AUTHORIZATIONWINDOW_H
#define AUTHORIZATIONWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "mainwindow.h"

namespace Ui {
class TAuthorizationWindow;
}

class TAuthorizationWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TAuthorizationWindow(QWidget *parent = nullptr);
    ~TAuthorizationWindow();

private:
    Ui::TAuthorizationWindow *ui;
    QTcpSocket* socket;
    QByteArray _Data;
    void SendToServer(QString str);

public slots:
    void SlotReadyRead();
private slots:
    void on_pushButtonAuthorization_clicked();
};

#endif // AUTHORIZATIONWINDOW_H
