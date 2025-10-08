#include "ApiClient.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

ApiClient::ApiClient(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &ApiClient::onReplyFinished);
}

void ApiClient::registerUser(const QString& username, const QString& password) {
    currentRequestType = "register";
    QNetworkRequest request(QUrl("http://localhost:18080/register"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["username"] = username;
    body["password"] = password;

    QJsonDocument doc(body);
    manager->post(request, doc.toJson());
}

void ApiClient::loginUser(const QString& username, const QString& password) {
    currentRequestType = "login";
    QNetworkRequest request(QUrl("http://localhost:18080/api/auth/login"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["username"] = username;
    body["password"] = password;

    QJsonDocument doc(body);
    manager->post(request, doc.toJson());
}

void ApiClient::refreshUserToken(const QString& oldToken) {
    currentRequestType = "refresh";
    QNetworkRequest request(QUrl("http://localhost:18080/refresh"));
    request.setRawHeader("Authorization", "Bearer " + oldToken.toUtf8());

    manager->post(request, QByteArray());
}

void ApiClient::logoutUser(const QString& token) {
    currentRequestType = "logout";
    QNetworkRequest request(QUrl("http://localhost:18080/logout"));
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());

    manager->post(request, QByteArray());
}

void ApiClient::revokeUserSession(const QString& adminToken, const QString& targetToken) {
    currentRequestType = "revoke";
    QNetworkRequest request(QUrl("http://localhost:18080/revoke"));
    request.setRawHeader("Authorization", "Bearer " + adminToken.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["token"] = targetToken;
    QJsonDocument doc(body);

    manager->post(request, doc.toJson());
}

void ApiClient::getUserProfile(const QString& username, const QString& token) {
    currentRequestType = "getUser";
    QNetworkRequest request(QUrl(QString("http://localhost:18080/api/users/%1").arg(username)));
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());

    manager->get(request);
}

void ApiClient::onReplyFinished(QNetworkReply* reply) {
    QByteArray responseData = reply->readAll();

    if (reply->error()) {
        qDebug() << "Request" << currentRequestType << "failed:" << reply->errorString();
    } else {
        qDebug() << "Request" << currentRequestType << "success. Response:" << responseData;
    }

    if (currentRequestType == "register") emit registerFinished(responseData);
    else if (currentRequestType == "login") emit loginFinished(responseData);
    else if (currentRequestType == "refresh") emit refreshFinished(responseData);
    else if (currentRequestType == "logout") emit logoutFinished(responseData);
    else if (currentRequestType == "revoke") emit revokeFinished(responseData);
    else if (currentRequestType == "getUser") emit getUserFinished(responseData);

    reply->deleteLater();
}
