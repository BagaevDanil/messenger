#ifndef TFORMMESSAGE_H
#define TFORMMESSAGE_H
#include <QWidget>
#include <QString>

class TFormMessage : public QWidget
{
    Q_OBJECT

public:
    TFormMessage(int id, QWidget *parent = nullptr);
    virtual void SetText(QString text);
    virtual void SetEditMark(bool val);
    const int& GetId() const;

protected:
    int _MsgId;
};

#endif // TFORMMESSAGE_H
