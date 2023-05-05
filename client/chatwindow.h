#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include <QVBoxLayout>

namespace Ui {
class TChatWindow;
}

class TChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TChatWindow(QString userLogin, QWidget *parent = nullptr);
    ~TChatWindow();
    bool HostExists();
    bool ConnectToHost();

private slots:
    void on_pushButtonSend_clicked();

private:
    Ui::TChatWindow *ui;
    QTcpSocket* _Socket;
    QByteArray _Data;
    bool Connected;
    int _CurInd;
    QString _UserLogin;
    void SendMsgToServer();
    QVBoxLayout* _Layout;
    QWidget* _Container;
    QPushButton* _Button;

public slots:
    void SlotReadyRead();
    void SlotSockDisc();
    void GetPackMessageEarly();

};

#endif // CHATWINDOW_H
