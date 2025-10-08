#ifndef AUTH_WINDOW_H
#define AUTH_WINDOW_H

#include <QDateTime>
#include <QMainWindow>
#include <QProgressBar>
#include <QTimer>

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

private:
    Ui::AuthWindow *ui;
    void initializeWindow();
    QTimer *progressTimer;
    void setupStyles();
    void setupLayout();
    void setupConnections();
    void handleSuccessfulLogin();
    void showPasswordChangeDialog();
    void openAdminWindow();
    void handleFailedLogin();
    QProgressBar *rateLimitProgress;
    int failedAttempts = 0;
    QDateTime lastAttemptTime;
    bool isRateLimited = false;
    QTimer *rateLimitTimer;
    const int MAX_ATTEMPTS = 5;
    const int LOCKOUT_TIME_MS = 300000;
    const int ATTEMPT_WINDOW_MS = 60000;
};
#endif //AUTH_WINDOW_H
