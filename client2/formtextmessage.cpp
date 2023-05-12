#include "formtextmessage.h"
#include "ui_formtextmessage.h"
#include <QTimer>
#include <QScrollBar>
#include <QSpacerItem>

TTextMessage::TTextMessage(TMessageData msg, bool isMyMsg, QWidget *parent)
    : ui(new Ui::TTextMessage)
{
    ui->setupUi(this);

    ui->labelLogin->setText(msg.Login);
    ui->labelTime->setText(msg.Time);
    _MsgId = msg.Ind;

    ui->textEdit->setText(msg.Text);
    QTimer::singleShot(2, this, &TTextMessage::onTextChanged);
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
}

void TTextMessage::SetText(QString text)
{
    ui->textEdit->setText(text);
}

void TTextMessage::PushButtonEdit()
{
    //qDebug() << "-Push EditButton";
    emit EditingMsg(_MsgId, ui->textEdit->toPlainText());
}

void TTextMessage::onTextChanged()
{
  QSize size = ui->textEdit->document()->size().toSize();
  ui->textEdit->setFixedHeight( size.height());
  ui->textEdit->verticalScrollBar()->setVisible(false);

}

TTextMessage::~TTextMessage()
{
    delete ui;
}
