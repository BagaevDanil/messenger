#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString userLogin, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonSend_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket* _Socket;
    QByteArray _Data;
    QString _UserLogin;
    void SendToServer(QString str);

public slots:
    void SlotReadyRead();
};
#endif // MAINWINDOW_H
