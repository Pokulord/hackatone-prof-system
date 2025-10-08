#ifndef AUTH_WINDOW_H
#define AUTH_WINDOW_H

#include <QDateTime>
#include <QMainWindow>
#include <QProgressBar>
#include <QTimer>
#include "api_client.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class AuthWindow;
}
QT_END_NAMESPACE

class AuthWindow : public QMainWindow
{
    Q_OBJECT

public:
    AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();

private slots:
    void onLoginClicked();
    void onShowPasswordToggled(bool checked);
    void activateRateLimit();
    void onRateLimitTimeout();
    void updateRateLimitUI();
    void updateRateLimitProgress();

    void onLoginResponse(const QByteArray& response); // Слот для ответа API

private:
    Ui::AuthWindow *ui;
    void initializeWindow();
    void setupStyles();
    void setupLayout();
    void setupConnections();

    void handleSuccessfulLogin();
    void showPasswordChangeDialog();
    void openAdminWindow();
    void handleFailedLogin();

    QProgressBar *rateLimitProgress;
    QTimer *rateLimitTimer;
    QTimer *progressTimer;

    int failedAttempts = 0;
    QDateTime lastAttemptTime;
    bool isRateLimited = false;

    const int MAX_ATTEMPTS = 5;
    const int LOCKOUT_TIME_MS = 300000;
    const int ATTEMPT_WINDOW_MS = 60000;

    ApiClient* apiClient;  // Интеграция API клиента
};

#endif // AUTH_WINDOW_H
