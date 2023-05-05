#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TServer server;
    if (!server.StartServer()) {
        return 0;
    }
    return a.exec();
}
