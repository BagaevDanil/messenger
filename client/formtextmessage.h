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
    static const int TIME_PAUSE_UPDATE_SIZE_TEXT;

public:
    TTextMessage(TMessageData msg, bool isMyMsg, bool isEditing, QWidget *parent = nullptr);
    ~TTextMessage();
    void SetText(QString text) override;
    void SetEditMark(bool val) override;

public slots:
    void onTextChanged();
    void PushButtonEdit();

signals:
    void EditingMsg(int msgId, QString text);

private:
    Ui::TTextMessage *ui;
};

#endif // FORMTEXTMESSAGE_H
