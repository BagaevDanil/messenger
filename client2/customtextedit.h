#ifndef TCUSTOMTEXTEDIT_H
#define TCUSTOMTEXTEDIT_H

#include <QPlainTextEdit>
#include <QKeyEvent>

class TCustomTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    TCustomTextEdit(QWidget *parent = nullptr) : QPlainTextEdit(parent) {}

signals:
    void sendMsg();

protected:
    void keyPressEvent(QKeyEvent *event) override;

};
#endif // TCUSTOMTEXTEDIT_H
