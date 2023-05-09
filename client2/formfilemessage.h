#ifndef FORMFILEMESSAGE_H
#define FORMFILEMESSAGE_H

#include <QWidget>
#include <QProgressBar>
#include "../common/common.h"

namespace Ui {
class TFormFileMessage;
}

class TFormFileMessage : public QWidget
{
    Q_OBJECT

public:
    explicit TFormFileMessage(TMessageData msg, bool isMyMsg, QWidget *parent = nullptr);
    ~TFormFileMessage();
    const int& GetFileID();
    const QString& GetFileName();
    QProgressBar* GetProgressBar();
    void FinishDownload();
    void UpdateDownload(int size);
    void StartDownload(int fileSize);

signals:
    void DownloadFile(TFormFileMessage* file);

private slots:
    void PushButton();

private:
    Ui::TFormFileMessage *ui;
    int _FileID;
    QString _FileName;
};

#endif // FORMFILEMESSAGE_H
