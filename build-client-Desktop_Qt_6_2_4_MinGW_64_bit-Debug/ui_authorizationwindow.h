/********************************************************************************
** Form generated from reading UI file 'authorizationwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTHORIZATIONWINDOW_H
#define UI_AUTHORIZATIONWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TAuthorizationWindow
{
public:
    QWidget *centralwidget;
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLineEdit *lineEditLogin;
    QLineEdit *lineEditPassword;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButtonAuthorization;
    QPushButton *pushButtonRegistration;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *TAuthorizationWindow)
    {
        if (TAuthorizationWindow->objectName().isEmpty())
            TAuthorizationWindow->setObjectName(QString::fromUtf8("TAuthorizationWindow"));
        TAuthorizationWindow->resize(551, 372);
        centralwidget = new QWidget(TAuthorizationWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(150, 110, 261, 121));
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lineEditLogin = new QLineEdit(widget);
        lineEditLogin->setObjectName(QString::fromUtf8("lineEditLogin"));

        verticalLayout->addWidget(lineEditLogin);

        lineEditPassword = new QLineEdit(widget);
        lineEditPassword->setObjectName(QString::fromUtf8("lineEditPassword"));

        verticalLayout->addWidget(lineEditPassword);


        verticalLayout_2->addLayout(verticalLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButtonAuthorization = new QPushButton(widget);
        pushButtonAuthorization->setObjectName(QString::fromUtf8("pushButtonAuthorization"));

        horizontalLayout->addWidget(pushButtonAuthorization);

        pushButtonRegistration = new QPushButton(widget);
        pushButtonRegistration->setObjectName(QString::fromUtf8("pushButtonRegistration"));

        horizontalLayout->addWidget(pushButtonRegistration);


        verticalLayout_2->addLayout(horizontalLayout);

        TAuthorizationWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(TAuthorizationWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 551, 26));
        TAuthorizationWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(TAuthorizationWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        TAuthorizationWindow->setStatusBar(statusbar);

        retranslateUi(TAuthorizationWindow);

        QMetaObject::connectSlotsByName(TAuthorizationWindow);
    } // setupUi

    void retranslateUi(QMainWindow *TAuthorizationWindow)
    {
        TAuthorizationWindow->setWindowTitle(QCoreApplication::translate("TAuthorizationWindow", "MainWindow", nullptr));
        lineEditLogin->setText(QString());
        pushButtonAuthorization->setText(QCoreApplication::translate("TAuthorizationWindow", "\320\222\321\205\320\276\320\264", nullptr));
        pushButtonRegistration->setText(QCoreApplication::translate("TAuthorizationWindow", "\320\240\320\265\320\263\320\270\321\201\321\202\321\200\320\260\321\206\320\270\321\217", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TAuthorizationWindow: public Ui_TAuthorizationWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTHORIZATIONWINDOW_H
