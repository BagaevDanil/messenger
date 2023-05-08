#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QProgressBar>
#include "../common/common.h"
#include "formfilemessage.h"

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
    void DownloadFileFromHost(TFormFileMessage* file);

private:
    Ui::TChatWindow *ui;
    QTcpSocket* _Socket;
    QByteArray _Data;
    bool Connected;
    int _CurInd;
    QString _UserLogin;
    QVBoxLayout* _Layout;
    QWidget* _Container;
    QPushButton* _Button;

    bool _Downloading;
    int  _FileByteSize;
    QByteArray _DataDownload;
    QString _FileNameDownload;
    TFormFileMessage* _FormFile;

    void AddNewMessage(TMessageData msg, bool toBottom);
    void DownloaIterations();

    template<class TypeData>
    void SendDataToServer(TypeData data, ETypeAction action);



public slots:
    void SlotReadyRead();
    void SlotSockDisc();
    void GetHistoryPack();
    void SetShiftHistory(int h);
    void ChangeVericalScroll(int x);

};

#endif // CHATWINDOW_H
