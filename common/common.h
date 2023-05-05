#ifndef COMMON_H
#define COMMON_H

#include <QString>

enum ETypeAction {
    AUTHORIZATION,
    MESSAGE,
    CHECK_CONNECTION,
};

namespace HOST {
    const QString ADDRES = "127.0.0.1";
    const int PORT = 1234;
};

#endif // COMMON_H
