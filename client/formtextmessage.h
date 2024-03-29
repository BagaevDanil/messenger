#ifndef FORMTEXTMESSAGE_H
#define FORMTEXTMESSAGE_H

#include <QWidget>
#include <QString>
#include "../common/common.h"
#include "formmessage.h"

namespace Ui {
class TTextMessage;
}

class TFormTextMessage : public TFormMessage
{
    Q_OBJECT

public:
    static const int TIME_PAUSE_UPDATE_SIZE_TEXT;

public:
    TFormTextMessage(TMessageData msg, bool isMyMsg, bool isEditing, QWidget *parent = nullptr);
    ~TFormTextMessage();
    void SetText(QString text) override;
    void SetEditMark(bool val) override;

public slots:
    void onTextChanged();
    void PushButtonEdit();

/*public slots:
    void CheckFieldOfView(int posY, int filedHeight);*/

signals:
    void EditingMsg(int msgId, QString text);

private:
    Ui::TTextMessage *ui;
};

#endif // FORMTEXTMESSAGE_H
