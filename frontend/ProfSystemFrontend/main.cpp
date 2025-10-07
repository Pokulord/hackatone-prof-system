#include "auth_window.h"
#include "usersInfo_window.h"
#include "admin_window.h"
#include "edituser_dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AuthWindow auth;
    auth.show();
    return a.exec();
}
