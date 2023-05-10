#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QProgressBar>
#include "../common/common.h"
#include "formfilemessage.h"
#include <QPlainTextEdit>
#include <QKeyEvent>
#include "customtextedit.h"

namespace Ui {
class TChatWindow;
}

class TChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    static const int HEIGHT_AUTOMATIC_SCROLL_DOWN;
    static const int TIME_AUTOMATIC_SCROLL_DOWN;
    static const int TIME_AUTOMATIC_SCROLL_HISTORY;
    static const int TIME_PAUSE_BEFORE_DOWNLOAD;
    static const int BYTE_DOWNLOAD_PACK_SIZE;
    static const int TIME_PAUSE_REQUEST_HISTORY;

public:
    explicit TChatWindow(QString userLogin, QWidget *parent = nullptr);
    ~TChatWindow();
    bool HostExists();
    bool ConnectToHost();

private slots:
    void on_pushButtonToBottom_clicked();
    void on_pushButton_clicked();
    void DownloadFileFromHost(TFormFileMessage* file);

private:
    Ui::TChatWindow *ui;
    TCustomTextEdit* _TextField;

    QTcpSocket* _Socket;
    QTcpSocket* _SocketDownload;

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

    QVector<QWidget> _MsgFeed;

private:
    void AddNewMessage(TMessageData msg, bool toBottom);
    void DownloaIterations();

    template<class TypeData>
    void SendDataToServer(TypeData data, ETypeAction action, QTcpSocket* socket = nullptr);

    void FinishLocalLoad();
    void UpdateLocalLoad(int size);
    void StartLocalLoad(int size);
    void SendFileToServer(QString fileName);

public slots:
    void TextFieldPress();
    void SlotReadyRead();
    void SlotSockDisc();
    void GetHistoryPack();
    void SetShiftHistory(int h);
    void ChangeVericalScroll(int x);

};

#endif // CHATWINDOW_H
