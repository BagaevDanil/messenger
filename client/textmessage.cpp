#include "textmessage.h"
#include "ui_textmessage.h"
#include <QTimer>
#include <QScrollBar>

TTextMessage::TTextMessage(TMessageData msg, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TTextMessage)
{
    ui->setupUi(this);

    // ui->labelText->setText(msg.Text);
    ui->labelLogin->setText(msg.Login);
    ui->labelTime->setText(msg.Time);

    ui->textEdit->setText(msg.Text);
    QTimer::singleShot(2, this, &TTextMessage::onTextChanged);

    //ui->textEdit->adjustSize();
    //ui->textEdit->setVisible(false);
}

void TTextMessage::onTextChanged()
{
  QSize size = ui->textEdit->document()->size().toSize();
  ui->textEdit->setFixedHeight( size.height());
  //ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->textEdit->verticalScrollBar()->setVisible(false);

}

TTextMessage::TTextMessage(QString login, QString text, QString time, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TTextMessage)
{
    ui->setupUi(this);

    // ui->labelText->setText(text);
    ui->labelLogin->setText(login);
    ui->labelTime->setText(time);

    //ui->labelText->setWordWrap(true);
    //ui->labelText->adjustSize();
    //ui->labelText->setFixedHeight(ui->labelText->sizeHint().height());

    //ui->textEdit->setPlainText(text);
    //ui->textEdit->setReadOnly(true);
    //ui->textEdit->setMinimumWidth(200);
    //ui->textEdit->setMinimumHeight(ui->textEdit->document()->size().height());

    //ui->textEdit->setPlainText(text);
    //QString s = ui->textEdit->toPlainText(); // нам понадобится сам текст
    //QFontMetrics m(ui->textEdit->font()); // и метрики шрифта
    //QRect textRect = m.boundingRect(QRect(0,0,0,0),Qt::TextWordWrap, s); // Найдем геометрические параметры текста (высота/ширина в пикселях) с учетом перевода строк
    // ui->textEdit->resize(textRect.width()+X,textRect.height()+X); // Изменяем размер виджета
    // ui->textEdit->setMinimumHeight(textRect.height());
}

TTextMessage::~TTextMessage()
{
    delete ui;
}
