#include "authorizationwindow.h"
#include "chatwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TAuthorizationWindow w;
    w.show();
    return a.exec();
}
