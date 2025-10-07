#ifndef USER_ROLES_H
#define USER_ROLES_H

#include <QString>
#include <QStringList>
#include <QMap>

class UserRoles
{
public:
    enum Role {
        User = 0,
        Operator,
        Moderator,
        Administrator
    };

    static QStringList getRoleList() {
        return {"Пользователь", "Оператор", "Модератор", "Администратор"};
    }

    static QString getRoleName(Role role) {
        switch(role) {
        case User: return "Пользователь";
        case Operator: return "Оператор";
        case Moderator: return "Модератор";
        case Administrator: return "Администратор";
        default: return "Пользователь";
        }
    }

    static Role getRoleFromName(const QString &roleName) {
        if (roleName == "Пользователь") return User;
        if (roleName == "Оператор") return Operator;
        if (roleName == "Модератор") return Moderator;
        if (roleName == "Администратор") return Administrator;
        return User;
    }
};

#endif // USER_ROLES_H
