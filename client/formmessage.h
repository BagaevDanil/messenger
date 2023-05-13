#ifndef TFORMMESSAGE_H
#define TFORMMESSAGE_H
#include <QWidget>
#include <QString>

class TFormMessage : public QWidget
{
    Q_OBJECT

public:
    TFormMessage(int id, bool isViewed, QWidget *parent = nullptr);
    virtual void SetText(QString text);
    virtual void SetEditMark(bool val);
    const int& GetId() const;

public slots:
    void CheckFieldOfView(int posY, int filedHeight);

signals:
    void ChangeViewStatus(int id);

protected:
    int _MsgId;
    bool _IsViewed;
    QWidget* _Label;
};

#endif // TFORMMESSAGE_H
