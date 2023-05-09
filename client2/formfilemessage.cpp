#include "formfilemessage.h"
#include "ui_formfilemessage.h"
#include <QTimer>

int TIME_PAUSE_FINISH_DOWNLOAD = 1000;

TFormFileMessage::TFormFileMessage(TMessageData msg, bool isMyMsg, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TFormFileMessage)
{
    ui->setupUi(this);

    ui->labelLogin->setText(msg.Login);
    ui->labelTime->setText(msg.Time);
    ui->pushButton->setText(msg.Text);
    ui->progressBar->setVisible(false);
    _FileID = msg.FileId;
    _FileName = msg.Text;
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(PushButton()));

    ui->horizontalLayout->removeItem(ui->horizontalSpacer);
    if (isMyMsg) {
        ui->horizontalLayout->insertSpacerItem(0, ui->horizontalSpacer);
        //ui->widget->setStyleSheet("");
        //ui->widget->setStyleSheet("background-color: #0055ff;color : rgb(255, 255, 255);border-radius: 10px;");
        //ui->widget->update();
    }
    else {
        ui->horizontalLayout->addSpacerItem(ui->horizontalSpacer);
        //ui->widget->setStyleSheet("");
        //ui->widget->setStyleSheet("background-color: #003399;color : rgb(255, 255, 255);border-radius: 10px;");
        //ui->widget->update();
    }
}

void TFormFileMessage::FinishDownload()
{
    ui->progressBar->setValue(ui->progressBar->maximum());
    QTimer::singleShot(TIME_PAUSE_FINISH_DOWNLOAD, this, [this](){
        ui->pushButton->setVisible(true);
        ui->progressBar->setVisible(false);
    });
}

void TFormFileMessage::UpdateDownload(int size)
{
    ui->progressBar->setValue(ui->progressBar->value() + size);
}

void TFormFileMessage::StartDownload(int fileSize)
{
    ui->pushButton->setVisible(false);

    ui->progressBar->setVisible(true);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(fileSize);
    ui->progressBar->setValue(0);
}

QProgressBar* TFormFileMessage::GetProgressBar()
{
    return ui->progressBar;
}

const int& TFormFileMessage::GetFileID()
{
    return _FileID;
}

const QString& TFormFileMessage::GetFileName()
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
