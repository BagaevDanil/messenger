#include "formtextmessage.h"
#include "ui_formtextmessage.h"
#include <QTimer>
#include <QScrollBar>
#include <QSpacerItem>

const int TFormTextMessage::TIME_PAUSE_UPDATE_SIZE_TEXT = 2;

TFormTextMessage::TFormTextMessage(TMessageData msg, bool isMyMsg, bool isEditing, QWidget *parent)
    : TFormMessage(msg.Ind, msg.IsViewed, parent)
    , ui(new Ui::TTextMessage)
{
    ui->setupUi(this);
    _Label = ui->viewWidget;

    ui->labelLogin->setText(msg.Login);
    ui->labelTime->setText(msg.Time);

    ui->textEdit->setText(msg.Text);
    QTimer::singleShot(TIME_PAUSE_UPDATE_SIZE_TEXT, this, &TFormTextMessage::onTextChanged);
    connect(ui->pushButtonEditing, SIGNAL(clicked()), this, SLOT(PushButtonEdit()));

    ui->horizontalLayout->removeItem(ui->horizontalSpacer);
    if (isMyMsg) {
        ui->horizontalLayout->insertSpacerItem(0, ui->horizontalSpacer);
        ui->pushButtonEditing->setVisible(true);
    }
    else {
        ui->horizontalLayout->addSpacerItem(ui->horizontalSpacer);
        ui->pushButtonEditing->setVisible(false);
    }

    if (isEditing) {
        ui->labelEditMark->setVisible(true);
    }
    else {
        ui->labelEditMark->setVisible(false);
    }

    if (_IsViewed) {
        _Label->setVisible(false);
    }
    else {
        _Label->setVisible(true);
    }
}

void TFormTextMessage::SetText(QString text)
{
    ui->textEdit->setText(text);
    QTimer::singleShot(TIME_PAUSE_UPDATE_SIZE_TEXT, this, &TFormTextMessage::onTextChanged);
}

void TFormTextMessage::SetEditMark(bool val)
{
    ui->labelEditMark->setVisible(val);
}

void TFormTextMessage::PushButtonEdit()
{
    emit EditingMsg(_MsgId, ui->textEdit->toPlainText());
}

void TFormTextMessage::onTextChanged()
{
  QSize size = ui->textEdit->document()->size().toSize();
  ui->textEdit->setFixedHeight( size.height());
  ui->textEdit->verticalScrollBar()->setVisible(false);

}

TFormTextMessage::~TFormTextMessage()
{
    delete ui;
}

/*void TFormTextMessage::CheckFieldOfView(int posY, int filedHeight)
{
    auto& rect = geometry();
    if (rect.y() <= posY + filedHeight && posY <= rect.y() + rect.height()) {
        qDebug() << "*Check Field Of View : " << filedHeight << posY << ui->textEdit->toPlainText();
    }
}*/
