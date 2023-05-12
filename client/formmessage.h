#ifndef TFORMMESSAGE_H
#define TFORMMESSAGE_H
#include <QWidget>
#include <QString>

class TFormMessage : public QWidget
{
    Q_OBJECT

public:
    TFormMessage(QWidget *parent = nullptr);
    virtual void SetText(QString text);
    virtual void SetEditMark(bool val);
};

#endif // TFORMMESSAGE_H
