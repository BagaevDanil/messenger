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

class CustomTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    CustomTextEdit(QWidget *parent = nullptr) : QPlainTextEdit(parent) {}
signals:
    void sendMsg();

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Return) {
            QTextCursor cursor = textCursor();
            cursor.insertBlock();
            setTextCursor(cursor);
        }
        else if (event->key() == Qt::Key_Return) {
            emit sendMsg();
        }
        else {
            QPlainTextEdit::keyPressEvent(event);
        }
    }
};

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
    void on_pushButtonToBottom_clicked();
    void on_pushButton_clicked();
    void DownloadFileFromHost(TFormFileMessage* file);

private:
    Ui::TChatWindow *ui;
    CustomTextEdit* _TextField;

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

    void AddNewMessage(TMessageData msg, bool toBottom);
    void DownloaIterations();

    template<class TypeData>
    void SendDataToServer(TypeData data, ETypeAction action, QTcpSocket* socket = nullptr);

    void FinishLocalLoad();
    void UpdateLocalLoad(int size);
    void StartLocalLoad(int size);

public slots:
    void TextFieldPress();
    void SlotReadyRead();
    void SlotSockDisc();
    void GetHistoryPack();
    void SetShiftHistory(int h);
    void ChangeVericalScroll(int x);

};

#endif // CHATWINDOW_H
