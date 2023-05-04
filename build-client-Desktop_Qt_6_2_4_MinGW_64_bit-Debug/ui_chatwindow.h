/********************************************************************************
** Form generated from reading UI file 'chatwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_H
#define UI_CHATWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TChatWindow
{
public:
    QMenuBar *menubar;
    QWidget *centralwidget;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *TChatWindow)
    {
        if (TChatWindow->objectName().isEmpty())
            TChatWindow->setObjectName(QString::fromUtf8("TChatWindow"));
        TChatWindow->resize(800, 600);
        menubar = new QMenuBar(TChatWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        TChatWindow->setMenuBar(menubar);
        centralwidget = new QWidget(TChatWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        TChatWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(TChatWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        TChatWindow->setStatusBar(statusbar);

        retranslateUi(TChatWindow);

        QMetaObject::connectSlotsByName(TChatWindow);
    } // setupUi

    void retranslateUi(QMainWindow *TChatWindow)
    {
        TChatWindow->setWindowTitle(QCoreApplication::translate("TChatWindow", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TChatWindow: public Ui_TChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
