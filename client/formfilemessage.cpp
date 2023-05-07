#include "formfilemessage.h"
#include "ui_formfilemessage.h"

TFormFileMessage::TFormFileMessage(TMessageData msg, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TFormFileMessage)
{
    ui->setupUi(this);

    ui->labelLogin->setText(msg.Login);
    ui->labelTime->setText(msg.Time);
    ui->pushButton->setText(msg.Text);
    _FileID = msg.FileId;
    _FileName = msg.Text;
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(PushButton()));
}

void TFormFileMessage::PushButton()
{
    emit DownloadFile(_FileID, _FileName);
}

TFormFileMessage::~TFormFileMessage()
{
    delete ui;
}
