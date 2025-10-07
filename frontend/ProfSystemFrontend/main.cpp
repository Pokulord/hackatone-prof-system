#include "auth_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AuthWindow auth;
    auth.show();
    return a.exec();
}
