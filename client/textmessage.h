#ifndef TEXTMESSAGE_H
#define TEXTMESSAGE_H

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
    TTextMessage(QString login, QString text, QString time, QWidget *parent = nullptr);
    TTextMessage(TMessageData msg, bool isMyMsg, QWidget *parent = nullptr);
    ~TTextMessage();

public slots:
    void onTextChanged();

private:
    Ui::TTextMessage *ui;
};

#endif // TEXTMESSAGE_H
