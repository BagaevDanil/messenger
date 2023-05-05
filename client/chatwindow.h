#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>

namespace Ui {
class TChatWindow;
}

class TChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TChatWindow(QString userLogin, QWidget *parent = nullptr);
    ~TChatWindow();
    void HostExists();
    bool ConnectToHost();

private slots:
    void on_pushButtonSend_clicked();

private:
    Ui::TChatWindow *ui;
    QTcpSocket* _Socket;
    QByteArray _Data;
    bool Connected;
    QString _UserLogin;
    void SendToServer(QString str);

public slots:
    void SlotReadyRead();
    void SlotSockDisc();

};

#endif // CHATWINDOW_H
