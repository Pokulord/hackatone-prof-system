#include "auth_window.h"
#include "usersInfo_window.h"
#include "admin_window.h"
#include "edituser_dialog.h"
#include "statistics_window.h"
#include "activeSessions_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ActiveSessionsWindow auth;
    auth.show();
    return a.exec();
}
