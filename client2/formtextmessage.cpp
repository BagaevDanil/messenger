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


    ui->pushButtonEditing->setVisible(isMyMsg);
    ui->labelEditStatus->setVisible(isEditing);
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
