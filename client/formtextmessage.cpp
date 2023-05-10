#include "formtextmessage.h"
#include "ui_formtextmessage.h"
#include <QTimer>
#include <QScrollBar>
#include <QSpacerItem>

TTextMessage::TTextMessage(TMessageData msg, bool isMyMsg, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TTextMessage)
{
    ui->setupUi(this);

    ui->labelLogin->setText(msg.Login);
    ui->labelTime->setText(msg.Time);

    ui->textEdit->setText(msg.Text);
    QTimer::singleShot(2, this, &TTextMessage::onTextChanged);

    ui->horizontalLayout->removeItem(ui->horizontalSpacer);
    if (isMyMsg) {
        ui->horizontalLayout->insertSpacerItem(0, ui->horizontalSpacer);
    }
    else {
        ui->horizontalLayout->addSpacerItem(ui->horizontalSpacer);
    }
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
