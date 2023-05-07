#ifndef FORMFILEMESSAGE_H
#define FORMFILEMESSAGE_H

#include <QWidget>
#include "../common/common.h"

namespace Ui {
class TFormFileMessage;
}

class TFormFileMessage : public QWidget
{
    Q_OBJECT

public:
    explicit TFormFileMessage(TMessageData msg, QWidget *parent = nullptr);
    ~TFormFileMessage();

signals:
    void DownloadFile(int id, QString fileName);

private slots:
    void PushButton();

private:
    Ui::TFormFileMessage *ui;
    int _FileID;
    QString _FileName;
};

#endif // FORMFILEMESSAGE_H
