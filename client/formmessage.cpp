#include "formmessage.h"
#include <QDebug>
#include <QTimer>
#include <QSpacerItem>
#include <QBoxLayout>

TFormMessage::TFormMessage(QWidget* parent) : QWidget(parent){}

void TFormMessage::SetText(QString /*text*/){}

void TFormMessage::SetEditMark(bool /*val*/){}

const int& TFormMessage::GetId() const
{
    return _MsgId;
}

void TFormMessage::SetupForm(TMessageData& msg, QWidget* viewWidget, QLabel* labelLogin, QLabel* labelTime, QBoxLayout* layout, bool isMyMsg)
{
    _ViewWidget = viewWidget;
    if (msg.IsViewed) {
        _ViewWidget->setVisible(false);
        _IsViewed = true;
    }
    else {
        _ViewWidget->setVisible(true);
        _IsViewed = false;
    }

    _MsgId = msg.Ind;

    _LabelLogin = labelLogin;
    _LabelLogin->setText(msg.Login);

    _LabelTime = labelTime;
    _LabelTime->setText(msg.Time);

    auto* _Spacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    if (isMyMsg) {
        layout->insertSpacerItem(0, _Spacer);
    }
    else {
        layout->addSpacerItem(_Spacer);
    }
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
            _ViewWidget->setVisible(false);
        });
    }
}
