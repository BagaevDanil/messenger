#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TServer Server;
    Server.startServer();

    return a.exec();
}
