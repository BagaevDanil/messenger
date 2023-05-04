#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>

namespace Ui {
class TChatWindow;
}

class TChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TChatWindow(QWidget *parent = nullptr);
    ~TChatWindow();

private:
    Ui::TChatWindow *ui;
};

#endif // CHATWINDOW_H
