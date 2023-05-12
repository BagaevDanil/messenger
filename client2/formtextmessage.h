#ifndef FORMTEXTMESSAGE_H
#define FORMTEXTMESSAGE_H

#include <QWidget>
#include <QString>
#include "../common/common.h"
#include "formmessage.h"

namespace Ui {
class TTextMessage;
}

class TTextMessage : public TFormMessage
{
    Q_OBJECT

public:
    TTextMessage(TMessageData msg, bool isMyMsg, QWidget *parent = nullptr);
    ~TTextMessage();
    void SetText(QString text);

public slots:
    void onTextChanged();
    void PushButtonEdit();

signals:
    void EditingMsg(int msgId, QString text);

private:
    Ui::TTextMessage *ui;
    int _MsgId;
};

#endif // FORMTEXTMESSAGE_H
