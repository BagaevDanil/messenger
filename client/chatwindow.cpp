#include "chatwindow.h"
#include "ui_chatwindow.h"

TChatWindow::TChatWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TChatWindow)
{
    ui->setupUi(this);
}

TChatWindow::~TChatWindow()
{
    delete ui;
}
