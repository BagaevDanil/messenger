#include "formfilemessage.h"
#include "ui_formfilemessage.h"
#include <QTimer>

const int TFormFileMessage::TIME_PAUSE_FINISH_DOWNLOAD = 1000;

TFormFileMessage::TFormFileMessage(TMessageData msg, bool isMyMsg, QWidget *parent)
    : TFormMessage()
    , ui(new Ui::TFormFileMessage)
{
    ui->setupUi(this);
    TFormMessage::SetupForm(msg, ui->viewWidget, ui->labelLogin, ui->labelTime, ui->horizontalLayout, isMyMsg);

    ui->pushButtonDownload->setText(msg.Text);
    ui->progressBarDownload->setVisible(false);
    _FileID = msg.FileId;
    _FileName = msg.Text;
    connect(ui->pushButtonDownload, SIGNAL(clicked()), this, SLOT(PushButton()));
}

void TFormFileMessage::SetText(QString text)
{
    ui->pushButtonDownload->setText(text);
}

void TFormFileMessage::FinishDownload()
{
    ui->progressBarDownload->setValue(ui->progressBarDownload->maximum());
    QTimer::singleShot(TIME_PAUSE_FINISH_DOWNLOAD, this, [this](){
        ui->pushButtonDownload->setVisible(true);
        ui->progressBarDownload->setVisible(false);
    });
}

void TFormFileMessage::UpdateDownload(int size)
{
    ui->progressBarDownload->setValue(ui->progressBarDownload->value() + size);
}

void TFormFileMessage::StartDownload(int fileSize)
{
    ui->pushButtonDownload->setVisible(false);

    ui->progressBarDownload->setVisible(true);
    ui->progressBarDownload->setMinimum(0);
    ui->progressBarDownload->setMaximum(fileSize);
    ui->progressBarDownload->setValue(0);
}

QProgressBar* TFormFileMessage::GetProgressBar()
{
    return ui->progressBarDownload;
}

const int& TFormFileMessage::GetFileID() const
{
    return _FileID;
}

const QString& TFormFileMessage::GetFileName() const
{
    return _FileName;
}

void TFormFileMessage::PushButton()
{
    emit DownloadFile(this);
}

TFormFileMessage::~TFormFileMessage()
{
    delete ui;
}
