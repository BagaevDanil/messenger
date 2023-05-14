#include "formtextmessage.h"
#include "ui_formtextmessage.h"
#include <QTimer>
#include <QScrollBar>
#include <QSpacerItem>
#include <QSpacerItem>

const int TFormTextMessage::TIME_PAUSE_UPDATE_SIZE_TEXT = 2;

TFormTextMessage::TFormTextMessage(TMessageData msg, bool isMyMsg, bool isEditing, QWidget *parent)
    : TFormMessage(parent)
    , ui(new Ui::TTextMessage)
{
    ui->setupUi(this);
    TFormMessage::SetupForm(msg, ui->viewWidget, ui->labelLogin, ui->labelTime, ui->horizontalLayout, isMyMsg);

    ui->textEdit->setText(msg.Text);
    QTimer::singleShot(TIME_PAUSE_UPDATE_SIZE_TEXT, this, &TFormTextMessage::onTextChanged);
    connect(ui->pushButtonEditing, SIGNAL(clicked()), this, SLOT(PushButtonEdit()));

    if (isMyMsg) {
        ui->pushButtonEditing->setVisible(true);
    }
    else {
        ui->pushButtonEditing->setVisible(false);
    }

    if (isEditing) {
        ui->labelEditStatus->setVisible(true);
    }
    else {
        ui->labelEditStatus->setVisible(false);
    }
}

void TFormTextMessage::SetText(QString text)
{
    ui->textEdit->setText(text);
    QTimer::singleShot(TIME_PAUSE_UPDATE_SIZE_TEXT, this, &TFormTextMessage::onTextChanged);
}

void TFormTextMessage::SetEditMark(bool val)
{
    ui->labelEditStatus->setVisible(val);
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
