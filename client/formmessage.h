#ifndef TFORMMESSAGE_H
#define TFORMMESSAGE_H
#include <QWidget>
#include <QString>
#include <QLabel>
#include <QBoxLayout>
#include "../common/common.h"

class TFormMessage : public QWidget
{
    Q_OBJECT

public:
    TFormMessage(QWidget *parent = nullptr);
    virtual void SetText(QString text);
    virtual void SetEditMark(bool val);
    const int& GetId() const;
    void SetupForm(TMessageData& msg, QWidget* viewWidget, QLabel* labelLogin, QLabel* labelTime, QBoxLayout* layout, bool isMyMsg);

public slots:
    void CheckFieldOfView(int posY, int filedHeight);

signals:
    void ChangeViewStatus(int id);

protected:
    int _MsgId;
    bool _IsViewed;
    QWidget* _ViewWidget;
    QLabel* _LabelLogin;
    QLabel* _LabelTime;
};

#endif // TFORMMESSAGE_H
