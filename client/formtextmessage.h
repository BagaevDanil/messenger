#ifndef FORMTEXTMESSAGE_H
#define FORMTEXTMESSAGE_H

#include <QWidget>
#include <QString>
#include "../common/common.h"

namespace Ui {
class TTextMessage;
}

class TTextMessage : public QWidget
{
    Q_OBJECT

public:
    TTextMessage(TMessageData msg, bool isMyMsg, QWidget *parent = nullptr);
    ~TTextMessage();

public slots:
    void onTextChanged();

private:
    Ui::TTextMessage *ui;
};

#endif // FORMTEXTMESSAGE_H
