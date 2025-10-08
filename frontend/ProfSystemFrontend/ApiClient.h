#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ApiClient : public QObject {
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr);

    void registerUser(const QString& username, const QString& password);
    void loginUser(const QString& username, const QString& password);
    void refreshUserToken(const QString& oldToken);
    void logoutUser(const QString& token);
    void revokeUserSession(const QString& adminToken, const QString& targetToken);
    void getUserProfile(const QString& username, const QString& token);

signals:
    void registerFinished(const QByteArray& response);
    void loginFinished(const QByteArray& response);
    void refreshFinished(const QByteArray& response);
    void logoutFinished(const QByteArray& response);
    void revokeFinished(const QByteArray& response);
    void getUserFinished(const QByteArray& response);

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* manager;
    QString currentRequestType;
};
