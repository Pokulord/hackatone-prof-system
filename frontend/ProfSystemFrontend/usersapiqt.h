#ifndef USERSAPIQT_H
#define USERSAPIQT_H

#pragma once

#include <QObject>
#include <QString>
#include <functional>

namespace UserApiUtils {
void changeUserPassword(
    QObject* context,
    const QString& username,
    const QString& newPassword,
    const QString& jwtToken,
    std::function<void(bool, const QString&)> callback
    );
}

#endif // USERSAPIQT_H
