#include "formmessage.h"

TFormMessage::TFormMessage(int id, QWidget* parent)
    : QWidget(parent)
    , _MsgId(id){}

void TFormMessage::SetText(QString /*text*/){}

void TFormMessage::SetEditMark(bool /*val*/){}

const int& TFormMessage::GetId() const
{
    return _MsgId;
}
