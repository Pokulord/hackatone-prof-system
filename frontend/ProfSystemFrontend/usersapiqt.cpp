#include "usersapiqt.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

namespace UserApiUtils {

void changeUserPassword(
    QObject* context,
    const QString& username,
    const QString& newPassword,
    const QString& jwtToken,
    std::function<void(bool, const QString&)> callback
    ) {
    QNetworkAccessManager* manager = new QNetworkAccessManager(context);
    QUrl url(QString("http://127.0.0.1:18080/api/users/%1").arg(username));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + jwtToken).toUtf8());

    QJsonObject payloadObj;
    payloadObj["password"] = newPassword;
    QJsonDocument doc(payloadObj);

    QNetworkReply* reply = manager->put(request, doc.toJson());

    QObject::connect(reply, &QNetworkReply::finished, context, [reply, callback, username]() {
        if (reply->error() == QNetworkReply::NoError) {
            callback(true, QString("Пароль пользователя %1 успешно изменён").arg(username));
        } else {
            callback(false, reply->errorString());
        }
        reply->deleteLater();
    });
}

}
