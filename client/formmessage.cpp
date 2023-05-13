#include "formmessage.h"
#include <QDebug>
#include <QTimer>

TFormMessage::TFormMessage(int id, bool isViewed, QWidget* parent)
    : QWidget(parent)
    , _MsgId(id)
    , _IsViewed(isViewed)
{

}

void TFormMessage::SetText(QString /*text*/){}

void TFormMessage::SetEditMark(bool /*val*/){}

const int& TFormMessage::GetId() const
{
    return _MsgId;
}

void TFormMessage::CheckFieldOfView(int posY, int filedHeight)
{
    if (_IsViewed) {
        return;
    }

    auto& rect = geometry();
    if (rect.y() <= posY + filedHeight && posY <= rect.y() + rect.height()) {
        // qDebug() << "*Check Field Of View : " << filedHeight << posY;

        _IsViewed = true;
        emit ChangeViewStatus(_MsgId);
        QTimer::singleShot(1200, [this] {
            _Label->setVisible(false);
        });
    }
}
