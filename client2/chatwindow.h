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
    void SendFileToServer(QString fileName);

private slots:
    void on_pushButtonSend_clicked();

    void on_pushButtonToBottom_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButton_clicked();

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
    void SetShiftHistory(int h);
    void MoveScroll(int x);

};

#endif // CHATWINDOW_H
